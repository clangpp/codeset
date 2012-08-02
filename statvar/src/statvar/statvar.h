// statvar.h
#ifndef STAT_VAR_H_
#define STAT_VAR_H_

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <string>
#include <vector>

template <typename AccumulateT,
	typename PreciseT = long double, typename CountT = std::size_t>
class StatVar {
public:
    typedef AccumulateT accumulate_type;
	typedef PreciseT precise_type;
    typedef CountT count_type;

public:
    std::string name;

public:
    StatVar(const std::string& var_name="")
		: name(var_name), sum_(), square_sum_(), min_(), max_(), counter_(),
		hist_enabled_(false) {
	}

    StatVar& operator += (const StatVar& other) {
        sum_ += other.sum_;
        square_sum_ += other.square_sum_;
		if (other.counter_ > 0 ) {
			if (counter_ > 0) {
				if (other.min_<min_) min_ = other.min_;
				if (other.max_>max_) max_ = other.max_;
			} else {
				min_ = other.min_;
				max_ = other.max_;
			}
		}
        counter_ += other.counter_;

		// histogram
		if (hist_enabled_ != other.hist_enabled_)
			throw std::runtime_error("histogram enable setting not match");
		if (hist_enabled_) {
			if (thresholds_ != other.thresholds_)
				throw std::runtime_error("histogram thresholds not match");
			std::transform(bins_.begin(), bins_.end(),
				other.bins_.begin(), bins_.begin(), std::plus<count_type>());
		}
        return *this;
    }

    void record(const accumulate_type& value) {
        sum_ += value;
        square_sum_ += value * value;
		if (counter_ > 0) {
			if (value<min_) min_ = value;
			if (value>max_) max_ = value;
		} else {
			min_ = max_ = value;
		}
        ++counter_;
		if (hist_enabled_) ++bins_[hist_bin_index(value)];
    }

	void clear() {
		sum_ = square_sum_ = min_ = max_ = 0;
		counter_ = 0;
		std::fill(bins_.begin(), bins_.end(), counter_);
	}

    StatVar& operator += (const accumulate_type& value) {
        record(value);
        return *this;
    }

    const accumulate_type& sum() const { return sum_; }

    const accumulate_type& min() const { return min_; }

    const accumulate_type& max() const { return max_; }

    const count_type& count() const { return counter_; }

	template <typename PreciseT>
    PreciseT mean() const {
        return static_cast<PreciseT>(sum_) / static_cast<PreciseT>(counter_);
    }

	template <typename PreciseT>
    PreciseT variance() const {
        PreciseT square_mean =
            static_cast<PreciseT>(square_sum_) / static_cast<PreciseT>(counter_);
        PreciseT record_mean = mean();
        return square_mean - record_mean * record_mean;
    }

	template <typename PreciseT>
    PreciseT deviation() const { return std::sqrt(variance()); }

	precise_type mean() const { return mean<precise_type>(); }

	precise_type variance() const { return variance<precise_type>(); }

	precise_type deviation() const { return deviation<precise_type>(); }

    friend std::ostream& operator << (std::ostream& out, const StatVar& var) {
        out << var.name
			<< " sum=" << var.sum()
			<< " count=" << var.count()
			<< " mean=" << var.mean()
			<< " min=" << var.min()
			<< " max=" << var.max()
			<< " variance=" << var.variance()
			<< " deviation=" << var.deviation();
		if (var.hist_enabled_) {
			out << "bin\tcount" << std::endl;
			out << "[less, " << var.thresholds_[0] << ")\t"
				<<  var.bins_[0] << std::endl;
			std::size_t i = 0;
			for (i=1; i<var.thresholds_.size(); ++i) {
				out << "[" << var.thresholds_[i-1] << ", "
					<< var.thresholds_[i] << ")\t" <<  var.bins_[i] << std::endl;
			}
			out << "[" << var.thresholds_[i-1] << ", more)\t"
				<< var.bins_[i] << std::endl;
		}
        return out;
    }

	template <typename InputIterator>
	void set_hist_thresholds(InputIterator first, InputIterator last) {
		thresholds_.assign(first, last);
		std::sort(thresholds_.begin(), thresholds_.end());

		// extra space for those values less than minimum threshold
		bins_.assign(thresholds.size()+1, 0.0);

		// no thresholds input to disable histogram
		hist_enabled_ = !thresholds_.empty();
	}

	void set_hist_linear_thresholds(
		accumulate_type start, accumulate_type step, accumulate_type stop) {
		std::vector<accumulate_type> values;
		for (accumulate_type value=start; value<=stop; value+=step)
			values.push_back(value);
		set_hist_thresholds(values.begin(), values.end());
	}

	// exponential thresholds,
	// value = times * std::pow(radix, power),
	// power in [pow_start, pow_stop] with step: pow_step
	void set_hist_exponential_thresholds(accumulate_type radix,
		accumulate_type pow_start, accumulate_type pow_step, accumulate_type pow_stop,
		accumulate_type times =1) {
		std::vector<accumulate_type> values;
		for (accumulate_type power=pow_start; power<=pow_stop; power+=pow_step)
			values.push_back( times * pow(radix, power) ) ;
		set_hist_thresholds(values.begin(), values.end());
	}

	const std::vector<accumulate_type>& hist_thresholds() const { return thresholds_; }

	const std::vector<count_type>& hist_bins() const { return bins_; }

	std::size_t hist_bin_count() const { return bins_.size(); }

	// locate histogram bin for value, use binary search
	std::size_t hist_bin_index(const accumulate_type& value) {
		// return std::upper_bound(thresholds.begin(), thresholds.end(), value) - thresholds.begin();

		// hand-write std::upper_bound(), because it runs too slow in debug mode.
		// binary search, find first position that is greater than 'value'
		size_t first = 0, last = thresholds_.size(), curr = first;
		while (first != last)
		{
			curr = (first + last) / 2;
			if (value < thresholds_[curr])  // curr in target range
			{
				last = curr;
			}
			else // (thresholds[curr] <= value) // curr not in target range
			{
				first = curr + 1;
			}
		}
		return first;
	}

private:
    accumulate_type sum_;
    accumulate_type square_sum_;
    accumulate_type min_;
    accumulate_type max_;
    count_type counter_;
	std::vector<accumulate_type> thresholds_;
	std::vector<count_type> bins_;
	bool hist_enabled_;
};

#endif  // STAT_VAR_H_

/**
 * 1. Please implement methods in class `BitSet`.
 *
 * 2. Run command line `java BitSetExercise`.
 *
 * 3. Find 'FAIL' and fix until all 'PASS'.
 */
package main;

class BitSet {
  // Adds bit into set.
  public static int add(int bitSet, int index) {
    return bitSet | (1 << index);
  }

  // Removes bit from set.
  public static int remove(int bitSet, int index) {
    return bitSet & ~(1 << index);
  }

  // Checks if set contains bit.
  public static boolean contains(int bitSet, int index) {
    return (bitSet & (1 << index)) != 0;
  }

  // Number of bits in set.
  public static int size(int bitSet) {
    int n = 0;
    for (; bitSet != 0; bitSet >>= 1) {
      n += (bitSet & 1);
    }
    return n;
  }

  // Checks if set is empty.
  public static boolean isEmpty(int bitSet) {
    return bitSet == 0;
  }

  // Removes all bits from set.
  public static int clear(int bitSet) {
    return 0;
  }
}

public class BitSetExercise {

  static class TestCase {
    public int bitSet;
    public int index;
    public int addResult;
    public int removeResult;
    public boolean containsResult;
    public int sizeResult;
    public boolean isEmptyResult;
    public int clearResult;

    public TestCase(
        int bitSet,
        int index,
        int addResult,
        int removeResult,
        boolean containsResult,
        int sizeResult,
        boolean isEmptyResult,
        int clearResult) {
      this.bitSet = bitSet;
      this.index = index;
      this.addResult = addResult;
      this.removeResult = removeResult;
      this.containsResult = containsResult;
      this.sizeResult = sizeResult;
      this.isEmptyResult = isEmptyResult;
      this.clearResult = clearResult;
    }
  }

  public static TestCase[] testCases =
      new TestCase[] {
        // Empty set {}.
        new TestCase(0b0000, 0, 0b0001, 0b0000, false, 0, true, 0),
        new TestCase(0b0000, 1, 0b0010, 0b0000, false, 0, true, 0),
        new TestCase(0b0000, 2, 0b0100, 0b0000, false, 0, true, 0),
        new TestCase(0b0000, 3, 0b1000, 0b0000, false, 0, true, 0),

        // Full set {0, 1, 2, 3}.
        new TestCase(0b1111, 0, 0b1111, 0b1110, true, 4, false, 0),
        new TestCase(0b1111, 1, 0b1111, 0b1101, true, 4, false, 0),
        new TestCase(0b1111, 2, 0b1111, 0b1011, true, 4, false, 0),
        new TestCase(0b1111, 3, 0b1111, 0b0111, true, 4, false, 0),

        // Set {0, 3}.
        new TestCase(0b1001, 0, 0b1001, 0b1000, true, 2, false, 0),
        new TestCase(0b1001, 1, 0b1011, 0b1001, false, 2, false, 0),
        new TestCase(0b1001, 2, 0b1101, 0b1001, false, 2, false, 0),
        new TestCase(0b1001, 3, 0b1001, 0b0001, true, 2, false, 0),

        // Set {1, 2}.
        new TestCase(0b0110, 0, 0b0111, 0b0110, false, 2, false, 0),
        new TestCase(0b0110, 1, 0b0110, 0b0100, true, 2, false, 0),
        new TestCase(0b0110, 2, 0b0110, 0b0010, true, 2, false, 0),
        new TestCase(0b0110, 3, 0b1110, 0b0110, false, 2, false, 0),

        // Set {0, 1}.
        new TestCase(0b0011, 0, 0b0011, 0b0010, true, 2, false, 0),
        new TestCase(0b0011, 1, 0b0011, 0b0001, true, 2, false, 0),
        new TestCase(0b0011, 2, 0b0111, 0b0011, false, 2, false, 0),
        new TestCase(0b0011, 3, 0b1011, 0b0011, false, 2, false, 0),

        // Set {0}.
        new TestCase(0b0001, 0, 0b0001, 0b0000, true, 1, false, 0),
        new TestCase(0b0001, 1, 0b0011, 0b0001, false, 1, false, 0),
        new TestCase(0b0001, 2, 0b0101, 0b0001, false, 1, false, 0),
        new TestCase(0b0001, 3, 0b1001, 0b0001, false, 1, false, 0),
      };

  public static void main(String[] args) {
    for (TestCase testCase : testCases) {
      System.out.println(
          "=== bitSet=0b"
              + Integer.toBinaryString(testCase.bitSet)
              + ", index="
              + testCase.index
              + " ===");

      // Adds bit into set.
      int addResult = BitSet.add(testCase.bitSet, testCase.index);
      System.out.println(
          (addResult == testCase.addResult ? "    PASS" : "FAIL")
              + ": add(0b"
              + Integer.toBinaryString(testCase.bitSet)
              + ", "
              + testCase.index
              + ")=0b"
              + Integer.toBinaryString(addResult)
              + ", want "
              + Integer.toBinaryString(testCase.addResult));

      // Removes bit from set.
      int removeResult = BitSet.remove(testCase.bitSet, testCase.index);
      System.out.println(
          (removeResult == testCase.removeResult ? "    PASS" : "FAIL")
              + ": remove(0b"
              + Integer.toBinaryString(testCase.bitSet)
              + ", "
              + testCase.index
              + ")=0b"
              + Integer.toBinaryString(removeResult)
              + ", want "
              + Integer.toBinaryString(testCase.removeResult));

      // Check if set contains bit.
      boolean containsResult = BitSet.contains(testCase.bitSet, testCase.index);
      System.out.println(
          (containsResult == testCase.containsResult ? "    PASS" : "FAIL")
              + ": contains(0b"
              + Integer.toBinaryString(testCase.bitSet)
              + ", "
              + testCase.index
              + ")="
              + containsResult
              + ", want "
              + testCase.containsResult);

      // Number of bits in set.
      int sizeResult = BitSet.size(testCase.bitSet);
      System.out.println(
          (sizeResult == testCase.sizeResult ? "    PASS" : "FAIL")
              + ": size(0b"
              + Integer.toBinaryString(testCase.bitSet)
              + ")="
              + sizeResult
              + ", want "
              + testCase.sizeResult);

      // Is set empty with no bits?
      boolean isEmptyResult = BitSet.isEmpty(testCase.bitSet);
      System.out.println(
          (isEmptyResult == testCase.isEmptyResult ? "    PASS" : "FAIL")
              + ": isEmpty(0b"
              + Integer.toBinaryString(testCase.bitSet)
              + ")="
              + isEmptyResult
              + ", want "
              + testCase.isEmptyResult);

      // Removes all bits from set.
      int clearResult = BitSet.clear(testCase.bitSet);
      System.out.println(
          (clearResult == testCase.clearResult ? "    PASS" : "FAIL")
              + ": clear(0b"
              + Integer.toBinaryString(testCase.bitSet)
              + ")=0b"
              + Integer.toBinaryString(clearResult)
              + ", want "
              + Integer.toBinaryString(testCase.clearResult));
    }
  }
}

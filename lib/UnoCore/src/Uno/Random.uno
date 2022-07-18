
namespace Uno
{
    public class Random
    {
        int z, w;

        public Random(int seed)
        {
            z = seed;
            w = seed + 1;
        }

        public int Next()
        {
            z = (36969 * (z & 0xFFFF) + (z >> 16)) & 0x7FFFFFF;
            w = (18000 * (w & 0xFFFF) + (w >> 16)) & 0x7FFFFFF;
            return ((z << 16) + w) & 0x7FFFFFF;
        }

        public int Next(int high)
        {
            if (high < 0)
                throw new ArgumentOutOfRangeException(nameof(high), "high cannot be negative");

            return (int)(NextDouble() * high);
        }

        public int Next(int low, int high)
        {
            if (low > high)
                throw new ArgumentOutOfRangeException("low cannot be greater than high");

            return low + (int)(NextDouble() * (high - low));
        }

        public double NextDouble()
        {
            return (double)Next() / 0x7FFFFFF;
        }
    }
}

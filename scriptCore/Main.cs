using System;

namespace Monado
{
    public class Main
    {

        public float FloatVar { get; set; }

        public Main()
        {
            Console.WriteLine("Main constructor");
        }

        public void PrintMessage()
        {
            Console.WriteLine("Hello World form C#!");
        }

        public void PrintCustomMessage(string message)
        {
            Console.WriteLine($"C# says: {message}");
        }

        public void PrintInt(int message)
        {
            Console.WriteLine($"Integer is: {message}");
        }

        public void PrintInts(int a, int b)
        {
            Console.WriteLine($"Integer a is: {a}");
            Console.WriteLine($"Integer b is: {b}");
        }
    }
}

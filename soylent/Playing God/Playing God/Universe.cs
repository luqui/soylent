using System;
using System.Collections.Generic;
using System.Text;

namespace Playing_God
{
    public class Universe
    {
        int width = 100;
        int height = 100;

        Cell[,] space;

        public Universe(int w, int h)
        {
            width = w;
            height = h;
            space = new Cell[width, height];
            for (int i = 0; i < width; i++)
            {
                for (int j = 0; j < height; j++)
                {
                    space[i, j] = new Cell();
                }
            }
        }

        public Cell LookUp(int i, int j)
        {
            return space[i, j];
        }

        public int Width { get { return space.GetLength(0); } }
        public int Height { get { return space.GetLength(1); } }
    }
}
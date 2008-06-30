using System;
using System.Collections.Generic;
using System.Text;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;

namespace Playing_God
{
    public class Cell
    {
        int element;

        public Cell()
        {
            element = 1;
            UpdateColor();
        }

        public int Element 
        { 
            get { return element; } 
            set 
            { 
                element = value;
                UpdateColor();
            } 
        }

        Vector3 color;
        public Color Color { get { return new Color(color); } }

        private void UpdateColor()
        {
            color = new Vector3(1.0f, 1.0f, 1.0f);
        }

    }
}

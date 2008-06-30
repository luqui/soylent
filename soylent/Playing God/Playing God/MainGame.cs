using System;
using System.Collections.Generic;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Audio;
using Microsoft.Xna.Framework.Content;
using Microsoft.Xna.Framework.GamerServices;
using Microsoft.Xna.Framework.Graphics;
using Microsoft.Xna.Framework.Input;
using Microsoft.Xna.Framework.Net;
using Microsoft.Xna.Framework.Storage;

namespace Playing_God
{
    /// <summary>
    /// This is the main type for your game
    /// </summary>
    public class MainGame : Microsoft.Xna.Framework.Game
    {
        GraphicsDeviceManager graphics;
        SpriteBatch spriteBatch;

        Universe universe;

        Texture2D pointTex;
        KeyboardState currentKeyState;
        KeyboardState lastKeyState;

        const int universeW = 40;
        const int universeH = 30;

        public MainGame()
        {
            graphics = new GraphicsDeviceManager(this);
            Content.RootDirectory = "Content";
        }

        protected override void Initialize()
        {
            base.Initialize();
        }

        protected override void LoadContent()
        {
            spriteBatch = new SpriteBatch(GraphicsDevice);
            pointTex = Content.Load<Texture2D>("point");

            universe = new Universe(universeW, universeH);
        }

        protected override void UnloadContent()
        {        }

        protected override void Update(GameTime gameTime)
        {
            // Allows the game to exit
            if ((GamePad.GetState(PlayerIndex.One).Buttons.Back == ButtonState.Pressed) || 
                Keyboard.GetState(PlayerIndex.One).IsKeyDown(Keys.Escape))
                this.Exit();

            CheckInput();

            base.Update(gameTime);
        }

        private void CheckInput()
        {
            lastKeyState = currentKeyState;
            currentKeyState = Keyboard.GetState(PlayerIndex.One);

            if (currentKeyState.IsKeyDown(Keys.F1) && lastKeyState.IsKeyUp(Keys.F1))
            {
                graphics.ToggleFullScreen();
            }
        }

        protected override void Draw(GameTime gameTime)
        {
            graphics.GraphicsDevice.Clear(Color.Black);

            spriteBatch.Begin(SpriteBlendMode.AlphaBlend);
            for (int i = 0; i < universe.Width; i++)
            {
                for (int j = 0; j < universe.Height; j++)
                {
                    Cell c = universe.LookUp(i, j);
                    float posx = i * (800.0f / universe.Width);
                    float posy = j * (600.0f / universe.Height);
                    spriteBatch.Draw(pointTex, new Vector2(posx, posy), null,
                        c.Color, 0.0f, new Vector2(), 0.3f, SpriteEffects.None, 0);
                }
            }
            spriteBatch.End();

            base.Draw(gameTime);
        }
    }
}

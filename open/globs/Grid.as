package {
	
	import flash.display.MovieClip;
	import flash.display.Stage;
	
	public class Grid extends MovieClip {
		public var gridWidth:Number    = 16.0;
		public var gridHeight:Number   = 10.0;
		public var markSpacing:Number  = 1.0;
		public var gridPxWidth:Number  = 800;
		public var gridPxHeight:Number = 500;
		
		public function x2px(inx:Number):Number {
			return (inx + gridWidth/2)  * gridPxWidth / gridWidth;
		}
		public function y2py(iny:Number):Number {
			return (-iny + gridHeight/2) * gridPxHeight / gridHeight;
		}
		public function px2x(inx:Number):Number {
			return inx * gridWidth  / gridPxWidth - gridWidth/2;
		}
		public function py2y(iny:Number):Number {
			return -(iny * gridHeight / gridPxHeight - gridHeight/2);
		}
		
		
		function Grid() {
			// draw main axes
			graphics.lineStyle(4, 0xff0000);
			graphics.moveTo(x2px(-gridWidth/2), y2py(0));
			graphics.lineTo(x2px( gridWidth/2), y2py(0));
			graphics.moveTo(x2px(0),            y2py(-gridHeight/2));
			graphics.lineTo(x2px(0),            y2py( gridHeight/2));
			
			var x:Number;
			var y:Number;
			// draw x axis marks
			graphics.lineStyle(1, 0xff0000);
			for (x = -gridWidth/2; x <= gridWidth/2; x += markSpacing) {
				graphics.moveTo(x2px(x), y2py(-0.2));
				graphics.lineTo(x2px(x), y2py(0.2));
			}
			
			// draw y axis marks
			for (y = -gridHeight/2; y <= gridHeight/2; y += markSpacing) {
				graphics.moveTo(x2px(-0.2), y2py(y));
				graphics.lineTo(x2px(0.2),  y2py(y));
			}
			
			// draw grid points
			for (x = -gridWidth/2; x <= gridWidth/2; x += markSpacing) {
				for (y = -gridHeight/2; y <= gridHeight/2; y += markSpacing) {
					graphics.drawCircle(x2px(x), y2py(y), 1);
				}
			}
		}
	}
	
}
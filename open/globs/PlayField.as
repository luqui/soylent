package {

	import flash.display.MovieClip;

	public class PlayField extends MovieClip {
		var plotter:Plotter;
		
		public function plot(e:Expression, callback:Function):void {
			if (plotter) {
				plotter.stop();
			}
			
			graphics.clear();
			plotter = new Plotter(e, this, 30.0, 5.0, callback);
			plotter.start();
		}
		
		public function stopPlot():void {
			if (plotter) {
				plotter.stop();
				plotter = null;
			}
		}
	}
	
}

import flash.utils.Timer;

class Plotter {
	var maxDelta:Number = 0.01;
	var minDelta:Number = 0.001;
	
	var expr:Expression;
	var curx:Number;
	var frameRate:Number;
	var drawRate:Number;
	var field:PlayField;
	var timer:Timer;
	var callback:Function;
	var stopd:Boolean;

	function Plotter(inexpr:Expression, infield:PlayField, inframeRate:Number, indrawRate:Number, incallback:Function) {
		expr = inexpr;
		field = infield;
		frameRate = inframeRate;
		drawRate = indrawRate;
		curx = -field.grid.gridWidth/2;
		callback = incallback;
		stopd = false;
	}
	
	function start() {
		timer = new Timer(1000/frameRate);
		timer.addEventListener("timer", function(e) { step() });
		timer.start();
	}
	
	function isUndef(inx:Number):Boolean {
		return isNaN(inx) || inx > field.grid.gridHeight/2 || inx < -field.grid.gridHeight/2;
	}
	
	function evalAt(inx:Number):Number {
		var pad:Object = new Object();
		pad.x = inx;
		return expr.run(pad);
	}
	
	function findDelta(inx:Number, crit:Function):Number {
		// assuming that expr(inx) does not satisfy crit, find the minimum
		// delta from inx that does satisfy crit
		var binsmin:Number;
		var binsmax:Number;
		
		var outy:Number;
		var del:Number = maxDelta;
		// step by max tolerance and see if we satisfy crit
		var newx:Number = inx + del;
		outy = evalAt(newx);
		
		if (!crit(outy)) {
			// no luck, let's step as large as we can
			return maxDelta;
		}
		else {
			// okay, let's find the boundary!
			binsmin = minDelta;
			binsmax = maxDelta;
			
			while (binsmax - binsmin > minDelta) {
				outy = evalAt(inx + (binsmin + binsmax)/2.0);
				if (crit(outy)) {
					binsmax = (binsmin + binsmax)/2.0;
				}
				else {
					binsmin = (binsmin + binsmax)/2.0;
				}
			}
			
			return binsmin;
		}
	}
	
	function step() {
		var grid:Grid = field.grid;
		
		field.graphics.lineStyle(2, 0x0000ff);
		
		var y:Number = evalAt(curx);
		var xstep:Number;
		var tmpy:Number;
		var tmpy2:Number;
		
		field.graphics.moveTo(grid.x2px(curx), grid.y2py(y));
		for (var x:Number = curx; !stopd && x < curx + drawRate / frameRate;) {
			if (isUndef(y)) {
				// find where this becomes defined
				xstep = findDelta(x, function (y:Number):Boolean { return !isUndef(y); });
				x += xstep;
				y = evalAt(x);
				field.graphics.moveTo(grid.x2px(x), grid.y2py(y));
			}
			else {
				// determine whether this becomes undefined
				xstep = findDelta(x, function (y:Number):Boolean { return isUndef(y); });
				tmpy = evalAt(x + xstep);
				if (isUndef(tmpy)) {
					x += xstep;
					y = tmpy;
					tmpy2 = evalAt(x-minDelta);
					field.graphics.lineTo(grid.x2px(x-minDelta), grid.y2py(tmpy2));
					if (callback != null) { callback(x-minDelta, tmpy2); }
				}
				else {
					// find the largest x within tolerance
					// (actually because of the design of my utility function, the
					//  smallest x not within tolerance... that's *almost* the same)
					xstep = findDelta(x, function (iny:Number):Boolean { return Math.abs(y - iny) >= maxDelta; });
					x += xstep;
					y = evalAt(x);
					field.graphics.lineTo(grid.x2px(x), grid.y2py(y));
					if (callback != null) { callback(x,y); }
				}
			}
		}
		curx = x;
		if (curx > grid.gridWidth/2) {
			timer.stop();
		}
	}
	
	function stop() {
		if (timer) { timer.stop(); }
		stopd = true;
	}
}
package {
	import flash.display.MovieClip;
	import flash.geom.Point;
	
	public class Game extends MovieClip {
		var nextScore = 1;
		var bonusFunc = function(x) { return x; };
		var globsLeft = 15;
		var numBlockers = 12;
		
		function Game() {			
			ui.fireButton.addEventListener("click", plotEvent);
			ui.funcInput.addEventListener("enter", aimEvent);
			ui.aimButton.addEventListener("click", aimEvent);
			ui.funcInput.addEventListener("change", textChangeEvent);
			stage.addEventListener("mouseMove", mouseMoveEvent);
			
			globField.seed(playField.grid, globsLeft, numBlockers);
		}
		
		function mouseMoveEvent(e) {
			ui.coordBox.text = "(" + playField.grid.px2x(e.stageX).toFixed(2) + ", " + playField.grid.py2y(e.stageY).toFixed(2) + ")";
		}
		
		function textChangeEvent(e) {
			var txt = ui.funcInput.text.replace(/\s/g, "");
			if (txt.length == 0) {
				bonusFunc = function(x) { return x; }
				ui.bonusText.text = "";
			}
			else if (txt.length <= 15) {
				bonusFunc = function(x) { return 2*x; }
				ui.bonusText.text = "" + txt.length + " chars: 2x bonus";
			}
			else if (txt.length <= 30) {
				bonusFunc = function(x) { return x+1; }
				ui.bonusText.text = "" + txt.length + " chars: +1 bonus";
			}
			else {
				bonusFunc = function(x) { return x; }
				ui.bonusText.text = "" + txt.length + " chars";
			}
		}
		
		function getExpr():Expression {
			try {
				return Expression.parse(ui.funcInput.text);
			}
			catch (e:SyntaxError) { 
				var errmsg = new ErrorMessage;
				errmsg.x = 200;
				errmsg.y = 150;
				errmsg.errorMsg.text = e.message;
				addChild(errmsg);
				return null;
			}
			trace("WTF!");
			return null;
		}

		function plotEvent(e) {
			var exp:Expression = getExpr();
			if (exp) {
				nextScore = 1;
				playField.plot(exp, whackGlob);
			}
		}

		function aimEvent(e) {
			var exp:Expression = getExpr();
			if (exp) {
				playField.plot(exp, null);
			}
		}
		
		function whackGlob(xPos:Number, yPos:Number):void {
			var px:Number = playField.grid.x2px(xPos);
			var py:Number = playField.grid.y2py(yPos);
			
			var objs:Array = globField.getObjectsUnderPoint(new Point(playField.grid.x2px(xPos), playField.grid.y2py(yPos)));
			
			for (var i:int = 0; i < objs.length; i++) {
				if (objs[i].parent is Glob) {  // .parent since we will hit the Shape object, not the movie clip
					var glob:Glob = objs[i].parent as Glob;
					if (!glob.popping) {
						glob.gotoAndPlay("pop");
						ui.scoreBox.text = (parseInt(ui.scoreBox.text) + nextScore).toString();
						playField.addChild(new BouncyNumber(nextScore, px, py));
						nextScore = bonusFunc(nextScore);
						
						globsLeft--;
						if (globsLeft == 0) {
							var restartBox = new RestartBox;
							restartBox.x = 200;  restartBox.y = 150;
							addChild(restartBox);
						}
					}
				}
				
				if (objs[i].parent is Antiglob) {
					playField.stopPlot();
				}
			}
		}
	}
}
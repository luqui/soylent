package {
	import flash.display.MovieClip;
	import flash.display.Sprite;
	
	public class StoneList extends MovieClip {
		private var stones:Object;
		private var container:Sprite;
		
		function StoneList() {
			container = new Sprite;
			addChild(container);
			
			stones = new Object;
			stones.top    = [];
			stones.bottom = [];
		}
		
		static function array_equals(a:Array, b:Array):Boolean {
			if (a.length != b.length) { return false; }
			
			var i:int;
			for (i = 0; i < a.length; i++) {
				if (a[i] != b[i]) { return false; }
			}
			return true;
		}
		
		public function randomize(max:int):void {
			var mkRandArray:Function = function ():Array {
				var ret:Array = [];
				var rand:int;
				while (ret.length < max && Math.random() >= 1.0/3.0) {
					ret.push(Math.round(Math.random()));
				}
				return ret;
			};
			
			var top:Array = [];
			var bottom:Array = [];
			while (array_equals(top, bottom)) {
				top = mkRandArray();
				bottom = mkRandArray();
			}
			
			insert(top, bottom);
		}
		
		public function insert(top:Array, bottom:Array):void {
			stones.top.push(top);
			stones.bottom.push(bottom);
			redraw();
		}
		
		static function concat(a:Array):Array {
			var ret:Array = [];
			a.forEach(function(b) { ret = ret.concat(b); });
			return ret;
		}
		
		public function insertList(other:StoneList):void {
			insert(concat(other.stones.top), concat(other.stones.bottom));
		}
		
		function removeAllChildren():void {
			var numchildren:int = container.numChildren;
			var i:int;
			for (i = 0; i < numchildren; i++) {
				container.removeChildAt(0);
			}
		}
		
		function drawStones(stoneList:Array, yPos:Number) {
			var xPos:Number = 25;
			var i:int;
			var j:int;
			var child:MovieClip;
			
			for (i = 0; i < stoneList.length; i++) {
				for (j = 0; j < stoneList[i].length; j++) {
					if (stoneList[i][j] == 0) {
						child = new BlackStone;
					}
					else {
						child = new WhiteStone;
					}
					child.x = xPos;
					child.y = yPos;
					xPos += 50;
					container.addChild(child);
				}
			}
		}
		
		function redraw():void {
			removeAllChildren();
			drawStones(stones.top, 25);
			drawStones(stones.bottom, 75);
		}
		
		public function undo():void {
			stones.top.pop();
			stones.bottom.pop();
			redraw();
		}
		
		public function checkWin():String {
			var topCt:Array = concat(stones.top);
			var botCt:Array = concat(stones.bottom);
			var i:int;
			for (i = 0; i < Math.min(topCt.length, botCt.length); i++) {
				if (topCt[i] != botCt[i]) {
					return "BAD";
				}
			}
			if (topCt.length == botCt.length && topCt.length > 0) {
				return "WIN";
			}
			else {
				return "OK";
			}
		}
	}
}
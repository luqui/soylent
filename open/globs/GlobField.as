package {
	
	import flash.display.MovieClip;
	
	public class GlobField extends MovieClip {
		public function seed(grid:Grid, numGlobs:int, numAntiglobs:int):void {
			var i:int;
			var posx, posy;
			
			for (i = 0; i < numAntiglobs; i++) {
				posx = Math.random() * grid.gridWidth - grid.gridWidth/2;
				posy = Math.random() * grid.gridHeight - grid.gridHeight/2;
				
				var aglob:Antiglob = new Antiglob;
				aglob.x = grid.x2px(posx);
				aglob.y = grid.y2py(posy);
				
				addChild(aglob);
			}
			
			for (i = 0; i < numGlobs; i++) {
				posx = Math.random() * grid.gridWidth - grid.gridWidth/2;
				posy = Math.random() * grid.gridHeight - grid.gridHeight/2;
				
				var glob:Glob = new Glob;
				glob.x = grid.x2px(posx);
				glob.y = grid.y2py(posy);
				
				addChild(glob);
			}
		}
	}
	
}
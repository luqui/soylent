package {
	import flash.display.MovieClip;
	import flash.text.TextField;
	
	public class BouncyNumber extends MovieClip {	
		function BouncyNumber(num:Number, inx:Number, iny:Number) {
			numberBox.text = num.toString();
			x = inx;  y = iny;
			addEventListener("enterFrame", onEnterFrame);
		}
		
		function onEnterFrame(e) {
			y -= 100.0/stage.frameRate;
			if (y < 0) {
				this.removeEventListener("enterFrame", onEnterFrame);
				parent.removeChild(this);
			}
		}
	}
}
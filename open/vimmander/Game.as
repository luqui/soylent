package {

import flash.display.MovieClip;
import flash.geom.Point;
	
public class Game extends MovieClip {
	public var m_dudes:Object = {};
	public var m_freeChars:Array = [
							 'a','s','d','f',
							 'q','w','e','r',
							 'z','x','c',
							 'j','k','l',
							 'u','i','o','p',
							 'n','m',
							 'g','h',
							 't','y',
							 'v','b'
							 ];

	public var m_state:Array;

	public static var game:Game;
	
	public function Game() {
		game = this;
		
		var self = this;
		addEventListener('enterFrame', step);
		stage.addEventListener('keyDown', function (e) {
			if (e.charCode == 27) { // escape
				resetUI();
			}
			else {
				self.m_state[0].procKey(String.fromCharCode(e.charCode));
			}
		});
		resetUI();
		
		makeDude('red', 350, 325);
		makeDude('red', 450, 325);
		makeDude('red', 400, 275);
	}
	
	public function resetUI():void {
		for each (var o:UIState in m_state) {
			o.cleanup();
		}
		m_state = [];
		pushUIState(new UISelectCharacterState());
	}
	
	public function pushUIState(st:UIState):void {
		m_state.unshift(st);
	}
	
	function step(e = null):void {
		for each (var o:Dude in m_dudes) {
			o.step();
		}
	}
	
	function makeDude(color:String, locx:Number, locy:Number):Dude {
		var dude:Dude = new Dude;
		if (color == "red") {
			dude.setGraphic(new RedDude);
		}
		else if (color == "blue") {
			dude.setGraphic(new BlueDude);
		}
		else {
			throw new Error("Color must be 'red' or 'blue' at this point");
		}
		
		dude.x = locx;
		dude.y = locy;
		
		dude.setId(makeId());
		
		m_dudes[dude.getId()] = dude;
		addChild(dude);
		return dude;
	}
	
	function makeId():String {
		for each (var ch:String in m_freeChars) {
			if (m_dudes[ch] == null) {
				return ch;
			}
		}
		return 'X';
	}
}

}

class UIState {
	public function procKey(key:String):void 
	{ }
	
	public function cleanup():void
	{ }
}

class UISelectCharacterState extends UIState {
	private var m_chars:Array = [];
	private var m_multiple:Boolean = false;
	
	public override function procKey(key:String):void {
		if (key == "'") {
			if (m_multiple) {
				// stop selection, next state
				Game.game.pushUIState(new UICommandState(m_chars));
 			}
			else {
				// enable multiple selection
				m_multiple = true;
			}
		}
		else if (/[a-z]/.test(key)) {
			var dude = Game.game.m_dudes[key];
			if (dude != null) {
				if (m_chars.indexOf(dude) != -1) {
					dude.hideSelect();
					m_chars.splice(m_chars.indexOf(dude), 1);
				}
				else {
					m_chars.push(dude);
					dude.showSelect();
				}
			}
			
			if (!m_multiple) {
				// character selected, next state
				Game.game.pushUIState(new UICommandState(m_chars));
			}
		}
		// otherwise ignore
	}
	
	public override function cleanup():void {
		for each (var i:Dude in m_chars) {
			i.hideSelect();
		}
	}
}

class UICommandState extends UIState {
	private var m_chars:Array = [];
	
	function UICommandState(chars:Array) {
		m_chars = chars;
	}
	
	public override function procKey(key:String):void {
		switch (key) {
			case 'm':
				Game.game.pushUIState(new UIMoveState(m_chars));
				break;
			default:
				// ignore
				break;
		}
	}
}

import flash.geom.Point;

class UIMoveState extends UIState {
	private var m_chars:Array = [];
	
	function UIMoveState(chars:Array) {
		m_chars = chars;
	}
	
	public override function procKey(key:String):void {
		var dir:Point = null;
		switch (key) {
			case 'w':
				dir = new Point(0, -1); break;
			case 'a':
				dir = new Point(-1, 0); break;
			case 's':
				dir = new Point(0, 1); break;
			case 'd':
				dir = new Point(1, 0); break;
		}
		if (dir != null) {
			for each (var c:Dude in m_chars) {
				c.setAction(Action.mk_MoveAction(dir, 30));
			}
		}
	}
}
package {

import flash.geom.Point;
	
public class Action {
	protected var m_dude:Dude = null;
	private var m_continuation:Function = null;
	
	public function setDude(dude:Dude):void {
		m_dude = dude;
	}
	
	public function setContinuation(cont):void {
		m_continuation = cont;
	}
	
	public function getContinuation():Function {
		return m_continuation;
	}
	
	public function begin():void 
	{ }
	
	public function step():void 
	{ }
	
	protected function finish():void {
		m_dude.setAction(null);  // set this _before_ continuation, so continuation can set a new one		
		if (m_continuation != null) {
			m_continuation();
		}
	}
	
	
	public static function mk_MoveAction(dir:Point, steps:int):Action {
		return new MoveAction(dir, steps);
	}
}

}

import flash.geom.Point;

class MoveAction extends Action
{
	private var m_dir:Point;
	private var m_stepsLeft:int;
	
	function MoveAction(dir:Point, steps:int) {
		m_dir = dir.clone();
		m_dir.normalize(1);
		m_stepsLeft = steps;
	}
	
	public override function step():void {
		if (m_stepsLeft-- == 0) {
			finish();
			return;
		}
		
		m_dude.x += m_dir.x;
		m_dude.y += m_dir.y;
	}
}

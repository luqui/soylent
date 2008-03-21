package {

import flash.display.MovieClip;
import flash.text.TextField;
import flash.geom.Point;

public class Dude extends MovieClip {
	private var m_id:String = '';
	private var m_health:Number = 1.0;
	private var m_graphic:MovieClip = null;
	private var m_action:Action = null;
	
	public function Dude() {
		hideSelect();
	}
	
	public function setId(new_id:String):void {
		m_id = new_id;
		if (this['idbox']) {
			this['idbox'].text = m_id;
		}
	}
	
	public function getId():String {
		return m_id;
	}
	
	public function setHealth(health:Number):void {
		m_health = health;
		if (this['healthbar']) {
			this['healthbar'].width = 29 * m_health;
		}
	}
	
	public function getHealth():Number {
		return m_health;
	}
	
	public function setGraphic(graphic:MovieClip):void {
		if (m_graphic != null) {
			removeChild(m_graphic);
		}
		m_graphic = graphic;
		m_graphic.x = 0;
		m_graphic.y = 0;
		addChild(m_graphic);
		swapChildren(m_graphic, idbox);
	}
	
	public function step(e = null):void {
		if (m_action) {
			m_action.step();
		}
	}
	
	public function showSelect():void {
		selectbox.visible = true;
	}
	
	public function hideSelect():void {
		selectbox.visible = false;
	}
	
	public function setAction(action:Action):void {
		m_action = action;
		if (m_action) {
			m_action.setDude(this);
			m_action.begin();
		}
	}
	
	public function getAction():Action {
		return m_action;
	}
}

}
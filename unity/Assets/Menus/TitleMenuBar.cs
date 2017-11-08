using UnityEngine;
using System.Collections;

public class TitleMenuBar : MonoBehaviour {
	private int index = 0;
	private int delta = -1;
	private float move = 0.0f;
	private float nextInputTime = 0.0f;
	
	void Awake() {
		Music.Singleton.playMenu();
	}
	
	void Update() {
		Color c = guiTexture.color;
		c.a += delta * Time.deltaTime * 0.5f;
		if(c.a < 0.2) {
			c.a = 0.2f;
			delta *= -1;
		}
		if(c.a > 0.5) {
			c.a = 0.5f;
			delta *= -1;
		}
		guiTexture.color = c;
		
		if(Input.GetKeyDown(KeyCode.Escape) || Input.GetButtonDown("Fire2")) 
			Application.Quit();
		
		if(Input.GetKeyDown(KeyCode.Return) || Input.GetButtonDown("Fire1")) {
			switch(index) {
			case 0:
				Application.LoadLevel("GameSelect");
				break;
			case 1:
				break;
			case 2:
				Application.Quit();
				break;
			}
		}

		if(Input.GetAxis("Vertical") == 0.0f)
			nextInputTime = 0.0f;
		
		if(move == 0.0f) {
			if(Time.time > nextInputTime) {
				if((Input.GetAxis("Vertical") <= -0.2f || Input.GetKeyDown((KeyCode)(int)OuyaKeyCode.JoystickButton9)) && index < 2) {
					move = -0.06f;
					index++;
					nextInputTime = Time.time + 0.5f;
				}
				if((Input.GetAxis("Vertical") >= 0.2f || Input.GetKeyDown((KeyCode)(int)OuyaKeyCode.JoystickButton8)) && index > 0) {
					move = 0.06f;
					index--;
					nextInputTime = Time.time + 0.5f;
				}
			}
		} else {
			Vector3 pos = transform.position;
			if(move > 0) {
				pos.y += 0.01f;
				move -= 0.01f;
			} else if(move < 0) {
				pos.y -= 0.01f;
				move += 0.01f;
			}
			transform.position = pos;
		}
	}
}

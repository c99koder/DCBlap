using UnityEngine;
using System.Collections;

public class GameSelect : MonoBehaviour {
	public GUITexture leftArrow;
	public GUITexture rightArrow;
	public Texture2D[] textures;
	private float nextInputTime = 0.0f;
	private int index = 0;
	
	void Awake() {
		Music.Singleton.playMenu();
	}

	void Update() {
		if(Input.GetKeyDown(KeyCode.Escape) || Input.GetButtonDown("Fire2")) 
			Application.LoadLevel("Title");
		
		if(Input.GetKeyDown(KeyCode.Return) || Input.GetButtonDown("Fire1"))
			OnMouseDown();
		
		if(Input.GetAxis("Horizontal") == 0.0f)
			nextInputTime = 0.0f;
		
		if(Time.time > nextInputTime) {
			if(Input.GetAxis("Horizontal") <= -0.2f || Input.GetKeyDown((KeyCode)(int)OuyaKeyCode.JoystickButton10)) {
				ArrowClicked ("arrow_left");
				nextInputTime = Time.time + 0.5f;
			}
			if(Input.GetAxis("Horizontal") >= 0.2f || Input.GetKeyDown((KeyCode)(int)OuyaKeyCode.JoystickButton11)) {
				ArrowClicked ("arrow_right");
				nextInputTime = Time.time + 0.5f;
			}
		}

	}
	
	void OnMouseDown() {
		switch(index) {
		case 0:
			Application.LoadLevel("BlapOut_Classic");
			break;
		case 1:
			break;
		}
	}
	
	void ArrowClicked(string arrow) {
		if(arrow.Equals("arrow_right"))
			index++;
		else
			index--;
		if(index < 0)
			index = 0;
		if(index >= textures.Length)
			index = textures.Length - 1;
		
		if(index == 0) {
			leftArrow.color = new Color(0.5f,0.5f,0.5f,0.25f);
			rightArrow.color = new Color(0.5f,0.5f,0.5f,0.5f);
		} else if(index == textures.Length - 1) {
			leftArrow.color = new Color(0.5f,0.5f,0.5f,0.5f);
			rightArrow.color = new Color(0.5f,0.5f,0.5f,0.25f);
		} else {
			leftArrow.color = new Color(0.5f,0.5f,0.5f,0.5f);
			rightArrow.color = new Color(0.5f,0.5f,0.5f,0.5f);
		}
		guiTexture.texture = textures[index];
	}
}

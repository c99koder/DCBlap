using UnityEngine;
using System.Collections;

public class TitleMenuText : MonoBehaviour {
	
	public GUITexture bar;
	
	void OnMouseDown() {
		OnMouseEnter();

		if(guiText.text.Equals("New Game"))
			Application.LoadLevel("GameSelect");
		
		if(guiText.text.Equals("Quit"))
			Application.Quit();
	}
	
	void OnMouseEnter() {
		Vector3 p = bar.transform.position;
		p.y = transform.position.y + 0.005f;
		bar.transform.position = p;
	}
}

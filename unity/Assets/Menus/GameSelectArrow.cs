using UnityEngine;
using System.Collections;

public class GameSelectArrow : MonoBehaviour {
	public GUITexture game;
	
	void OnMouseDown() {
		game.SendMessage("ArrowClicked", gameObject.name);
	}
}

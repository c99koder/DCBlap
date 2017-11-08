using UnityEngine;
using System.Collections;

public class Score : MonoBehaviour {
	
	public static int score = 0;
	
	// Use this for initialization
	void Start () {
		score = 0;
	}
	
	// Update is called once per frame
	void OnGUI () {
		guiText.text = "Score: " + score;
	}
}

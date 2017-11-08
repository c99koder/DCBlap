using UnityEngine;
using System.Collections;

public class TitleCamera : MonoBehaviour {
	
	private float delta = 0.4f;
	private float fov = 0.0f;
	private float start = 0.0f;
	
	// Use this for initialization
	void Start () {
		start = camera.fov;
	}
	
	// Update is called once per frame
	void Update () {
		fov += delta * Time.deltaTime;
		if(fov > 5 || fov < 0)
			delta *= -1;
		camera.fov = start - fov;
	}
}

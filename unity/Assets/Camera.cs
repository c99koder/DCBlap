using UnityEngine;
using System.Collections;

public class Camera : MonoBehaviour {
	float start;
	
	// Use this for initialization
	void Start () {
		start = camera.fov;
		camera.fov += 10;
	}
	
	// Update is called once per frame
	void Update () {
		if(camera.fov > start)
			camera.fov-=0.1f;
	}
}

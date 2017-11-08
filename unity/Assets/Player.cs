using UnityEngine;
using System.Collections;

public class Player : MonoBehaviour {
	
	public OuyaSDK.OuyaPlayer player;
	
	// Use this for initialization
	void Start () {
		Color c = renderer.material.color;
		c.a = 0.0f;
		renderer.material.color = c;
	}
	
	// Update is called once per frame
	void Update () {
		Color c = renderer.material.color;
		if(c.a < 1.0f) {
			c.a += 0.01f;
			renderer.material.color = c;
		}
		if(Input.GetKeyDown(KeyCode.Escape) || Input.GetButtonDown("Fire2")) 
			Application.LoadLevel("Title");
	}
	
	void FixedUpdate() {
		if(renderer.material.color.a >= 0.9f) {
#if UNITY_ANDROID
			transform.Translate(new Vector3(Input.GetAxis(string.Format("Joy{0} Axis 1", (int)player)),0,0) * -6.0f * Time.fixedDeltaTime);
#else
			transform.Translate(new Vector3(Input.GetAxis("Horizontal"),0,0) * -6.0f * Time.fixedDeltaTime);
#endif
			if (Input.touchCount > 0) {
			    Touch touch = Input.GetTouch(0);
			    if (touch.position.x < Screen.width/2) {
					transform.Translate(new Vector3(-1.0f,0f,0f) * -6.0f * Time.fixedDeltaTime);
			    } else if (touch.position.x > Screen.width/2) {
					transform.Translate(new Vector3(1.0f,0f,0f) * -6.0f * Time.fixedDeltaTime);
			    }
			}
		}
	}
	
	void OnCollisionExit(Collision collisionInfo) {
		rigidbody.velocity = Vector3.zero;
	}
}

using UnityEngine;
using System.Collections;

public class Ball : MonoBehaviour {
	Vector3 start;
	int delay;
	
	public AudioClip bounce;
	public AudioClip score;
	public AudioClip win;
	
	void Awake() {
		Music.Singleton.playGame();
	}
	
	// Use this for initialization
	void Start () {
		Color c = renderer.material.color;
		c.a = 0.0f;
		renderer.material.color = c;
		delay = 3;
		start = transform.position;
		Reset();
	}
	
	void Reset () {
		transform.position = start;
		rigidbody.velocity = Vector3.zero;
		StartCoroutine(Launch());
	}
	
	IEnumerator Launch () {
		yield return new WaitForSeconds(delay);
		delay = 1;
		
		Vector3 v;
		if(transform.rotation.y == 0)
			v = new Vector3(Random.Range(-4.0f, 4.0f),0,-4);
		else
			v = new Vector3(Random.Range(-4.0f, 4.0f),0,4);
		v.x += Random.Range(-0.2f, 0.2f);
		v.z += Random.Range(-0.2f, 0.2f);

		if(v.x > 2.0f)
			v.x = 2.0f;
		
		if(v.x < -2.0f)
			v.x = -2.0f;
		
		if(v.z > 4.0f)
			v.z = 4.0f;

		if(v.z < -4.0f)
			v.z = -4.0f;
		
		rigidbody.velocity = v;
	}
	
	void Update() {
		Color c = renderer.material.color;
		if(c.a < 1.0f) {
			c.a += 0.01f;
			renderer.material.color = c;
		}
	}
	
	void OnCollisionEnter(Collision collisionInfo) {
		Vector3 v = rigidbody.velocity;
		
		if(v.x != 0.0 || v.z != 0) {
			if(v.x >= 0.0 && v.x < 2.0f) {
				v.x = 2.0f;
			}
			if(v.x < 0.0 && v.x > -2.0f) {
				v.x = -2.0f;
			}
			if(v.z >= 0.0f && v.z < 2.0f) {
				v.z = 2.0f;
			}
			if(v.z < 0.0f && v.z > -2.0f) {
				v.z = -2.0f;
			}
			
			v.x += Random.Range(-0.1f, 0.1f);
			v.z += Random.Range(-0.1f, 0.1f);
		}

		if(v.x > 4.0f)
			v.x = 4.0f;
		
		if(v.x < -4.0f)
			v.x = -4.0f;
		
		if(v.z > 4.0f)
			v.z = 4.0f;

		if(v.z < -4.0f)
			v.z = -4.0f;
		
		rigidbody.velocity = v;

		if(collisionInfo.gameObject.tag == "Breakable") {
			Score.score+=collisionInfo.gameObject.GetComponent<Breakable>().points;
			Destroy(collisionInfo.gameObject);
			if(GameObject.FindGameObjectsWithTag("Breakable").Length == 1) {
				audio.PlayOneShot(win);
				Destroy(gameObject);
			} else {
				audio.PlayOneShot(bounce);
			}
		} else if(collisionInfo.gameObject.tag == "Goal") {
			Reset();
			audio.PlayOneShot(score);
		} else {
			audio.PlayOneShot(bounce);
		}
	}
}

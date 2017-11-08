using UnityEngine;
using System.Collections;

public class Breakable : MonoBehaviour {
	
	public int points = 10;
	private Vector3 startPos;
	
	// Use this for initialization
	void Start () {
		if(rigidbody) {
			rigidbody.useGravity = false;
			rigidbody.isKinematic = true;
		}
		startPos = transform.position;
		transform.Translate(0,0,4.0f + (points / 10));
	}
	
	// Update is called once per frame
	void Update () {
		if(transform.position.y > startPos.y) {
			transform.Translate(0,0,-4 * Time.deltaTime);
		} else if(!rigidbody || !rigidbody.useGravity) {
			transform.position = startPos;
			if(rigidbody) {
				rigidbody.useGravity = true;
				rigidbody.isKinematic = false;
			}
		}
	}
}

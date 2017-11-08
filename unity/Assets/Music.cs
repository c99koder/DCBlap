using UnityEngine;
using System.Collections;

public class Music : MonoBehaviour {
    private static Music m_instance = null;

	public AudioClip menu;
	public AudioClip game;
	
	public static Music Singleton {
        get {
			if(null == m_instance) {
                GameObject o = GameObject.Find("Music");
                if(o) {
                    m_instance = o.GetComponent<Music>();
                }
            }
			return m_instance;
        }
    }

	void Awake () {
		if(m_instance != null && m_instance != this) {
			Destroy(gameObject);
			return;
		}
		m_instance = this;
		DontDestroyOnLoad(gameObject);
	}
	
	public void playMenu() {
		if(!audio.clip.Equals(menu)) {
			if(audio.isPlaying)
				audio.Stop();
			audio.clip = menu;
			audio.Play();
		}
	}

	public void playGame() {
		if(!audio.clip.Equals(game)) {
			if(audio.isPlaying)
				audio.Stop();
			audio.clip = game;
			audio.Play();
		}
	}
	
	public void stop() {
		audio.Stop();
	}
}

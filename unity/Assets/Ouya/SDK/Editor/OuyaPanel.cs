using System;
using System.Diagnostics;
using System.IO;
using System.Collections.Generic;
using System.Text;
using System.Threading;
using System.Xml;
using UnityEditor;
using UnityEngine;
using Debug = UnityEngine.Debug;

public class OuyaPanel : EditorWindow
{
    private const string KEY_POST_PROCESSOR = "OuyaPanelUsePostProcessor";

    public static bool UsePostProcessor
    {
        get
        {
            if (EditorPrefs.HasKey(OuyaPanel.KEY_POST_PROCESSOR))
            {
                return EditorPrefs.GetBool(OuyaPanel.KEY_POST_PROCESSOR);
            }
            else
            {
                return false;
            }
        }
        set
        {
            EditorPrefs.SetBool(OuyaPanel.KEY_POST_PROCESSOR, value);
        }
    }

    private static string[] m_toolSets =
        {
            "OUYA",
            "Unity",
            "Java JDK",
            "Android SDK",
            "Android NDK"
        };

    private int m_selectedToolSet = 0;

    #region Operations

    private bool m_toggleRunApplication = false;

    private bool m_toggleBuildApplication = false;

    private bool m_toggleBuildAndRunApplication = false;

    private bool m_toggleBuildRunAndCompileApplication = false;

    private bool m_toggleCompileJava = false;

    private bool m_toggleCompilePlugin = false;

    private bool m_toggleCompileNDK = false;

    private bool m_toggleSyncBundleID = false;

    private bool m_toggleOpenAndroidSDK = false;

    #endregion

    #region OUYA SDK

    public const string KEY_PATH_OUYA_SDK = @"OUYA SDK";
    public const string KEY_PATH_JAR_GUAVA = @"Guava Jar";
    public const string KEY_PATH_JAR_GSON = @"GSON Jar";
    public const string KEY_PATH_JAR_OUYA_UNITY_PLUGIN = @"OUYA/Plugin Jar";
    public const string KEY_JAVA_APP_NAME = @"OuyaJavaAppName";
    public const string KEY_APK_NAME = @"OuyaJavaApkName";

    private static string pathOuyaSDKJar = string.Empty;
    private static string pathGsonJar = string.Empty;
    private static string pathGuavaJar = string.Empty;
    private static string pathOuyaUnityPluginJar = string.Empty;

    private static string pathManifestPath = string.Empty;
    private static string pathRes = string.Empty;
    private static string pathBin = string.Empty;
    private static string pathSrc = string.Empty;

    private static string javaAppName = "OuyaUnityApplication";
    private static string apkName = "OuyaUnityApplication.apk";

    void UpdateOuyaPaths()
    {
        pathOuyaSDKJar = string.Format("{0}/Assets/Plugins/Android/libs/ouya-sdk.jar", pathUnityProject);
        pathGsonJar = string.Format("{0}/Assets/Plugins/Android/libs/gson-2.2.2.jar", pathUnityProject);
        pathGuavaJar = string.Format("{0}/Assets/Plugins/Android/libs/guava-r09.jar", pathUnityProject);
        pathOuyaUnityPluginJar = string.Format("{0}/Assets/Plugins/Android/OuyaUnityPlugin.jar", pathUnityProject);

        pathManifestPath = string.Format("{0}/Assets/Plugins/Android/AndroidManifest.xml", pathUnityProject);
        pathRes = string.Format("{0}/Assets/Plugins/Android/res", pathUnityProject);
        pathBin = string.Format("{0}/Assets/Plugins/Android/bin", pathUnityProject);
        pathSrc = string.Format("{0}/Assets/Plugins/Android/src", pathUnityProject);

        EditorPrefs.SetString(KEY_PATH_OUYA_SDK, pathOuyaSDKJar);
    }

    static string GetRJava()
    {
        if (string.IsNullOrEmpty(PlayerSettings.bundleIdentifier))
        {
            return string.Empty;
        }

        string path = string.Format("Assets/Plugins/Android/src/{0}/R.java", PlayerSettings.bundleIdentifier.Replace(".", "/"));
        FileInfo fi = new FileInfo(path);
        return fi.FullName;
    }

    private static string GetApplicationJava()
    {
        string path = string.Format("Assets/Plugins/Android/src/{0}.java", javaAppName);
        FileInfo fi = new FileInfo(path);
        return fi.FullName;
    }

    private static DateTime timerCheckAppJavaPackageName = DateTime.MinValue;
    private static string applicationJavaPackageName = string.Empty;
    private static string GetApplicationJavaPackageName()
    {
        string path = GetApplicationJava();

        if (!File.Exists(path))
        {
            return "NOT_FOUND";
        }

        if (timerCheckAppJavaPackageName < DateTime.Now)
        {
            timerCheckAppJavaPackageName = DateTime.Now + TimeSpan.FromSeconds(1);
            try
            {
                using (StreamReader sr = new StreamReader(GetApplicationJava()))
                {
                    applicationJavaPackageName = sr.ReadLine();
                }
            }
            catch (System.Exception)
            {
            }
        }

        return applicationJavaPackageName;
    }

    private static DateTime timerCheckManifestPackageName = DateTime.MinValue;
    private static string androidManifestPackageName = string.Empty;
    private static string GetAndroidManifestPackageName()
    {
        if (!File.Exists(pathManifestPath))
        {
            return "NOT_FOUND";
        }

        if (timerCheckManifestPackageName < DateTime.Now)
        {
            timerCheckManifestPackageName = DateTime.Now + TimeSpan.FromSeconds(1);
            try
            {
                XmlDocument xDoc = new XmlDocument();
                xDoc.Load(pathManifestPath);
                foreach (XmlNode level1 in xDoc.ChildNodes)
                {
                    if (level1.Name.ToUpper() == "MANIFEST")
                    {
                        XmlElement element = (XmlElement) level1;
                        foreach (XmlAttribute attribute in element.Attributes)
                        {
                            if (attribute.Name.ToUpper() == "PACKAGE")
                            {
                                androidManifestPackageName = attribute.Value;
                            }
                        }
                    }
                }
            }
            catch (System.Exception)
            {
            }
        }

        return androidManifestPackageName;
    }

    string GetIOuyaActivityJava()
    {
        string path = "Assets/Plugins/Android/src/IOuyaActivity.java";
        FileInfo fi = new FileInfo(path);
        return fi.FullName;
    }

    static string GetBundlePrefix()
    {
        string identifier = PlayerSettings.bundleIdentifier;
        if (string.IsNullOrEmpty(identifier))
        {
            return string.Empty;
        }

        foreach (string data in identifier.Split(".".ToCharArray()))
        {
            return data;
        }

        return string.Empty;
    }

    #endregion

    #region Android SDK

    public const string KEY_PATH_ANDROID_JAR = @"Android Jar";
    public const string KEY_PATH_ANDROID_ADB = @"ADB Path";
    public const string KEY_PATH_ANDROID_AAPT = @"APT Path";
    public const string KEY_PATH_ANDROID_SDK = @"SDK Path";

    public const string REL_ANDROID_PLATFORM_TOOLS = "platform-tools";
    public const string FILE_AAPT_WIN = "aapt.exe";
    public const string FILE_AAPT_MAC = "aapt";
    public const string FILE_ADB_WIN = "adb.exe";
    public const string FILE_ADB_MAC = "adb";

    public static string pathADB = string.Empty;
    public static string pathAAPT = string.Empty;
    public static string pathSDK = string.Empty;

    static string GetPathAndroidJar()
    {
        return string.Format("{0}/platforms/android-{1}/android.jar", pathSDK, (int)PlayerSettings.Android.minSdkVersion);
    }

    void UpdateAndroidSDKPaths()
    {
        switch (Application.platform)
        {
            case RuntimePlatform.OSXEditor:
                pathADB = string.Format("{0}/{1}/{2}", pathSDK, REL_ANDROID_PLATFORM_TOOLS, FILE_ADB_MAC);
                pathAAPT = string.Format("{0}/{1}/{2}", pathSDK, REL_ANDROID_PLATFORM_TOOLS, FILE_AAPT_MAC);
                break;
            case RuntimePlatform.WindowsEditor:
                pathADB = string.Format("{0}/{1}/{2}", pathSDK, REL_ANDROID_PLATFORM_TOOLS, FILE_ADB_WIN);
                pathAAPT = string.Format("{0}/{1}/{2}", pathSDK, REL_ANDROID_PLATFORM_TOOLS, FILE_AAPT_WIN);
                break;
        }

        EditorPrefs.SetString(KEY_PATH_ANDROID_SDK, pathSDK);
    }

    void ResetAndroidSDKPaths()
    {
        switch (Application.platform)
        {
            case RuntimePlatform.OSXEditor:
                pathSDK = @"android-sdk-mac_x86";
                break;
            case RuntimePlatform.WindowsEditor:
                pathSDK = @"C:/Program Files (x86)/Android/android-sdk";
                break;
        }

        UpdateAndroidSDKPaths();
    }

    void SelectAndroidSDKPaths()
    {
        string path = string.Empty;
        switch (Application.platform)
        {
            case RuntimePlatform.OSXEditor:
                path = EditorUtility.OpenFolderPanel(string.Format("Path to {0}", KEY_PATH_ANDROID_SDK), pathSDK, "../android-sdk-mac_x86");
                break;
            case RuntimePlatform.WindowsEditor:
                path = EditorUtility.OpenFolderPanel(string.Format("Path to {0}", KEY_PATH_ANDROID_SDK), pathSDK, @"..\android-sdk");
                break;
        }
        if (!string.IsNullOrEmpty(path))
        {
            pathSDK = path;
        }

        UpdateAndroidSDKPaths();
    }

    #endregion

    #region Android NDK

    private const string KEY_PATH_ANDROID_NDK = @"NDK Path";
    private const string KEY_PATH_ANDROID_NDK_MAKE = @"NDK Make";
    private const string KEY_PATH_ANDROID_JNI_MK = @"JNI mk";
    private const string KEY_PATH_ANDROID_JNI_CPP = @"JNI cpp";
    private const string KEY_PATH_OUYA_NDK_LIB = @"OUYA NDK Lib";

    public static string pathNDK = string.Empty;
    public static string pathNDKMake = string.Empty;
    public static string pathJNIMk = string.Empty;
    public static string pathJNIMkTemp = string.Empty;
    public static string pathJNICpp = string.Empty;
    public static string pathObj = string.Empty;
    public static string pathOuyaNDKLib = string.Empty;

    void UpdateAndroidNDKPaths()
    {
        pathObj = string.Format("{0}/Assets/Plugins/Android/obj", pathUnityProject);
        pathJNIMk = string.Format("{0}/Assets/Plugins/Android/jni/OuyaAndroid.mk", pathUnityProject);
        pathJNIMkTemp = string.Format("{0}/Assets/Plugins/Android/jni/Android.mk", pathUnityProject);

        switch (Application.platform)
        {
            case RuntimePlatform.OSXEditor:
                pathNDKMake = string.Format("{0}/prebuilt/darwin-x86/bin/make", pathNDK);
                pathJNICpp = string.Format("{0}/Assets/Plugins/Android/jni/jni.cpp", pathUnityProject);
                pathOuyaNDKLib = string.Format("{0}/Assets/Plugins/Android/libs/armeabi/lib-ouya-ndk.so", pathUnityProject);
                break;
            case RuntimePlatform.WindowsEditor:
                pathNDKMake = string.Format("{0}/prebuilt/windows-x86_64/bin/make.exe", pathNDK);
                pathJNICpp = string.Format("{0}/Assets/Plugins/Android/jni/jni.cpp", pathUnityProject);
                pathOuyaNDKLib = string.Format("{0}/Assets/Plugins/Android/libs/armeabi/lib-ouya-ndk.so", pathUnityProject);
                break;
        }

        EditorPrefs.SetString(KEY_PATH_ANDROID_NDK, pathNDK);
    }

    void ResetAndroidNDKPaths()
    {
        switch (Application.platform)
        {
            case RuntimePlatform.OSXEditor:
                pathNDK = @"android-ndk-r8d";
                break;
            case RuntimePlatform.WindowsEditor:
                pathNDK = @"android-ndk-r8d";
                break;
        }

        UpdateAndroidNDKPaths();
    }

    void SelectAndroidNDKPaths()
    {
        string path = string.Empty;
        switch (Application.platform)
        {
            case RuntimePlatform.OSXEditor:
                path = EditorUtility.OpenFolderPanel(string.Format("Path to {0}", KEY_PATH_ANDROID_NDK), pathNDK, "../android-ndk-r8c");
                break;
            case RuntimePlatform.WindowsEditor:
                path = EditorUtility.OpenFolderPanel(string.Format("Path to {0}", KEY_PATH_ANDROID_NDK), pathNDK, @"..\android-ndk-r8e");
                break;
        }
        if (!string.IsNullOrEmpty(path))
        {
            pathNDK = path;
            UpdateAndroidNDKPaths();
        }
    }

    public static void CompileNDK()
    {
        if (!File.Exists(pathNDKMake))
        {
            return;
        }

        if (!File.Exists(pathJNICpp))
        {
            return;
        }

        if (!File.Exists(pathJNIMk))
        {
            return;
        }

        File.Copy(pathJNIMk, pathJNIMkTemp, true);

        if (File.Exists(pathOuyaNDKLib))
        {
            File.Delete(pathOuyaNDKLib);
        }

        switch (Application.platform)
        {
            case RuntimePlatform.OSXEditor:
                RunProcess(pathNDKMake, "Assets/Plugins/Android",
                    string.Format("-f \"{0}/build/core/build-local.mk\" -ouya-ndk",
                        pathNDK.Replace(@"/", "/")));
                break;
            case RuntimePlatform.WindowsEditor:
                RunProcess(pathNDKMake, "Assets/Plugins/Android",
                    string.Format("-f \"{0}/build/core/build-local.mk\" -ouya-ndk",
                        pathNDK.Replace(@"/", "/")));
                break;
        }

        if (File.Exists(pathJNIMkTemp))
        {
            File.Delete(pathJNIMkTemp);
        }

        if (Directory.Exists(pathObj))
        {
            Directory.Delete(pathObj, true);
        }
    }

    public static void SyncBundleID()
    {
        string bundleId = PlayerSettings.bundleIdentifier;
        if (string.IsNullOrEmpty(bundleId))
        {
            return;
        }

        try
        {
            string path = GetApplicationJava();
            if (File.Exists(path))
            {
                StringBuilder sb = new StringBuilder();
                sb.AppendFormat("package {0};", bundleId);
                sb.AppendLine();

                using (StreamReader sr = new StreamReader(GetApplicationJava()))
                {
                    int count = 0;
                    string line;
                    do
                    {
                        line = sr.ReadLine();
                        if (null != line)
                        {
                            if (count > 0)
                            {
                                sb.Append(line);
                                sb.AppendLine();
                            }
                        }
                        ++count;
                    } while (null != line);
                }

                using (StreamWriter sw = new StreamWriter(GetApplicationJava()))
                {
                    sw.Write(sb.ToString());
                }
            }
            else
            {
                Debug.LogError(string.Format("Unable to find application java: {0}", path));
            }
        }
        catch (System.Exception)
        {
        }

        try
        {
            string path = pathManifestPath;
            if (File.Exists(path))
            {
                XmlDocument xDoc = new XmlDocument();
                xDoc.Load(path);

                foreach (XmlNode level1 in xDoc.ChildNodes)
                {
                    if (level1.Name.ToUpper() == "MANIFEST")
                    {
                        XmlElement element = (XmlElement)level1;
                        foreach (XmlAttribute attribute in element.Attributes)
                        {
                            if (attribute.Name.ToUpper() == "PACKAGE")
                            {
                                attribute.Value = bundleId;
                            }
                        }
                    }
                }

                xDoc.Save(path);
            }
            else
            {
                Debug.LogError(string.Format("Unable to find android manifest: {0}", path));
            }
        }
        catch (System.Exception)
        {
        }
    }

    #endregion

    #region Unity Paths

    public const string KEY_PATH_UNITY_JAR = @"Unity Jar";
    public const string KEY_PATH_UNITY_EDITOR = @"Unity Editor";
    public const string KEY_PATH_UNITY_PROJECT = @"Unity Project";

    public const string PATH_UNITY_JAR_WIN = "Data/PlaybackEngines/androidplayer/bin/classes.jar";
    public const string PATH_UNITY_JAR_MAC = "Contents/PlaybackEngines/AndroidPlayer/bin/classes.jar";

    private static string pathUnityJar = string.Empty;
    private static string pathUnityEditor = string.Empty;
    private static string pathUnityProject = string.Empty;

    void UpdateUnityPaths()
    {
        switch (Application.platform)
        {
            case RuntimePlatform.OSXEditor:
                pathUnityJar = string.Format("{0}/{1}", EditorApplication.applicationPath, PATH_UNITY_JAR_MAC);
                break;
            case RuntimePlatform.WindowsEditor:
                pathUnityJar = string.Format("{0}/{1}", pathUnityEditor, PATH_UNITY_JAR_WIN);
                break;
        }
    }

    #endregion

    #region Java JDK

    public const string KEY_PATH_JAVA_TOOLS_JAR = @"Tools Jar";
    public const string KEY_PATH_JAVA_JAR = @"Jar Path";
    public const string KEY_PATH_JAVA_JAVAC = @"JavaC Path";
    public const string KEY_PATH_JAVA_JAVAP = @"SDK Path";
    public const string KEY_PATH_JAVA_JDK = @"JDK Path";

    public const string REL_JAVA_PLATFORM_TOOLS = "bin";
    public const string FILE_JAR_WIN = "jar.exe";
    public const string FILE_JAR_MAC = "jar";
    public const string FILE_JAVAC_WIN = "javac.exe";
    public const string FILE_JAVAC_MAC = "javac";
    public const string FILE_JAVAP_WIN = "javap.exe";
    public const string FILE_JAVAP_MAC = "javap";

    public static string pathToolsJar = string.Empty;
    public static string pathJar = string.Empty;
    public static string pathJavaC = string.Empty;
    public static string pathJavaP = string.Empty;
    public static string pathJDK = string.Empty;

    void UpdateJavaJDKPaths()
    {
        switch (Application.platform)
        {
            case RuntimePlatform.OSXEditor:
                pathToolsJar = string.Format("{0}/Contents/Classes/classes.jar", pathJDK);
                pathJar = string.Format("{0}/Contents/Commands/{1}", pathJDK, FILE_JAR_MAC);
                pathJavaC = string.Format("{0}/Contents/Commands/{1}", pathJDK, FILE_JAVAC_MAC);
                pathJavaP = string.Format("{0}/Contents/Commands/{1}", pathJDK, FILE_JAVAP_MAC);
                break;
            case RuntimePlatform.WindowsEditor:
                pathToolsJar = string.Format("{0}/lib/tools.jar", pathJDK);
                pathJar = string.Format("{0}/{1}/{2}", pathJDK, REL_JAVA_PLATFORM_TOOLS, FILE_JAR_WIN);
                pathJavaC = string.Format("{0}/{1}/{2}", pathJDK, REL_JAVA_PLATFORM_TOOLS, FILE_JAVAC_WIN);
                pathJavaP = string.Format("{0}/{1}/{2}", pathJDK, REL_JAVA_PLATFORM_TOOLS, FILE_JAVAP_WIN);
                break;
        }

        EditorPrefs.SetString(KEY_PATH_JAVA_JDK, pathJDK);
    }

    void ResetJavaJDKPaths()
    {
        switch (Application.platform)
        {
            case RuntimePlatform.OSXEditor:
                pathJDK = @"/System/Library/Java/JavaVirtualMachines/1.6.0.jdk";
                break;
            case RuntimePlatform.WindowsEditor:
                pathJDK = @"C:\Program Files (x86)/Java/jdk1.6.0_37";
                break;
        }

        UpdateJavaJDKPaths();
    }

    void SelectJavaJDKPaths()
    {
        string path = string.Empty;
        switch (Application.platform)
        {
            case RuntimePlatform.OSXEditor:
                path = EditorUtility.OpenFolderPanel(string.Format("Path to {0}", KEY_PATH_JAVA_JDK), pathJDK, "../jdk1.6.0_37");
                break;
            case RuntimePlatform.WindowsEditor:
                path = EditorUtility.OpenFolderPanel(string.Format("Path to {0}", KEY_PATH_JAVA_JDK), pathJDK, @"..\jdk1.6.0_37");
                break;
        }
        if (!string.IsNullOrEmpty(path))
        {
            pathJDK = path;
        }

        UpdateJavaJDKPaths();
    }

    #endregion

    [MenuItem("Window/Open Ouya Panel")]
    private static void MenuOpenPanel()
    {
        GetWindow<OuyaPanel>("Ouya Panel");
    }

    void OnEnable()
    {
        if (EditorPrefs.HasKey(KEY_JAVA_APP_NAME))
        {
            javaAppName = EditorPrefs.GetString(KEY_JAVA_APP_NAME);
        }

        if (EditorPrefs.HasKey(KEY_APK_NAME))
        {
            apkName = EditorPrefs.GetString(KEY_APK_NAME);
        }

        pathUnityEditor = new FileInfo(EditorApplication.applicationPath).Directory.FullName;
        pathUnityProject = new DirectoryInfo(Directory.GetCurrentDirectory()).FullName;
        UpdateUnityPaths();

        if (EditorPrefs.HasKey(KEY_PATH_ANDROID_SDK))
        {
            pathSDK = EditorPrefs.GetString(KEY_PATH_ANDROID_SDK);
        }

        if (string.IsNullOrEmpty(pathSDK))
        {
            ResetAndroidSDKPaths();
        }
        else
        {
            UpdateAndroidSDKPaths();
        }


        if (EditorPrefs.HasKey(KEY_PATH_ANDROID_NDK))
        {
            pathNDK = EditorPrefs.GetString(KEY_PATH_ANDROID_NDK);
        }

        if (string.IsNullOrEmpty(pathNDK))
        {
            ResetAndroidNDKPaths();
        }
        else
        {
            UpdateAndroidNDKPaths();
        }


        if (EditorPrefs.HasKey(KEY_PATH_JAVA_JDK))
        {
            pathJDK = EditorPrefs.GetString(KEY_PATH_JAVA_JDK);
        }

        if (string.IsNullOrEmpty(pathJDK))
        {
            ResetJavaJDKPaths();
        }
        else
        {
            UpdateJavaJDKPaths();
        }

        UpdateOuyaPaths();
    }

    void Update()
    {
        Repaint();

        if (!string.IsNullOrEmpty(m_nextScene))
        {
            EditorApplication.OpenScene(m_nextScene);
            m_nextScene = string.Empty;
            return;
        }

        var scenes = EditorBuildSettings.scenes;
        var sceneList = new List<string>();

        foreach (var scene in scenes)
        {
            if (scene.enabled)
                sceneList.Add(scene.path);
        }

        var sceneArray = sceneList.ToArray();

        if (m_toggleCompilePlugin)
        {
            m_toggleCompilePlugin = false;

            OuyaMenuAdmin.MenuGeneratePluginJar();

            AssetDatabase.Refresh();
        }

        if (m_toggleCompileJava)
        {
            m_toggleCompileJava = false;

            if (CompileApplicationJava())
            {
                AssetDatabase.Refresh();
            }
        }

        if (m_toggleCompileNDK)
        {
            m_toggleCompileNDK = false;

            CompileNDK();

            AssetDatabase.Refresh();
        }

        if (m_toggleSyncBundleID)
        {
            m_toggleSyncBundleID = false;

            SyncBundleID();

            AssetDatabase.Refresh();
        }

        if (m_toggleRunApplication)
        {
            m_toggleRunApplication = false;

            string appPath = string.Format("{0}/{1}", pathUnityProject, apkName);
            if (File.Exists(appPath))
            {
                //Debug.Log(appPath);
                //Debug.Log(pathADB);
                string args = string.Format("shell am start -n {0}/.{1}", PlayerSettings.bundleIdentifier, javaAppName);
                //Debug.Log(args);
                ProcessStartInfo ps = new ProcessStartInfo(pathADB, args);
                Process p = new Process();
                p.StartInfo = ps;
                p.Exited += (object sender, EventArgs e) =>
                                {
                                    p.Dispose();
                                };
                p.Start();
            }
        }

        if (m_toggleBuildApplication)
        {
            m_toggleBuildApplication = false;

            if (GenerateRJava())
            {
                if (CompileApplicationClasses())
                {
                    if (BuildApplicationJar())
                    {
                        AssetDatabase.Refresh();

                        BuildPipeline.BuildPlayer(sceneArray, string.Format("{0}/{1}", pathUnityProject, apkName),
                                                  BuildTarget.Android, BuildOptions.None);
                    }
                }
            }
        }

        if (m_toggleBuildAndRunApplication)
        {
            m_toggleBuildAndRunApplication = false;

            AssetDatabase.Refresh();

            BuildOptions options = BuildOptions.AutoRunPlayer;
            if (EditorUserBuildSettings.allowDebugging)
            {
                options |= BuildOptions.Development | BuildOptions.AllowDebugging;
            }

            BuildPipeline.BuildPlayer(sceneArray, string.Format("{0}/{1}", pathUnityProject, apkName),
                                      BuildTarget.Android, options);
        }

        if (m_toggleBuildRunAndCompileApplication)
        {
            m_toggleBuildRunAndCompileApplication = false;

            if (GenerateRJava())
            {
                if (CompileApplicationClasses())
                {
                    if (BuildApplicationJar())
                    {
                        AssetDatabase.Refresh();

                        BuildOptions options = BuildOptions.AutoRunPlayer;
                        if (EditorUserBuildSettings.allowDebugging)
                        {
                            options |= BuildOptions.Development | BuildOptions.AllowDebugging;
                        }

                        BuildPipeline.BuildPlayer(sceneArray, string.Format("{0}/{1}", pathUnityProject, apkName),
                                                  BuildTarget.Android, options);
                    }
                }
            }
        }

        if (m_toggleOpenAndroidSDK)
        {
            m_toggleOpenAndroidSDK = false;

            string androidPath = string.Empty;
            
            switch (Application.platform)
            {
                case RuntimePlatform.OSXEditor:
                    androidPath = string.Format("{0}/tools/android", pathSDK);
                    break;
                
                case RuntimePlatform.WindowsEditor:
                    androidPath = string.Format("{0}/tools/android.bat", pathSDK);
                    break;
            }

            if (!string.IsNullOrEmpty(androidPath) &&
                File.Exists(androidPath))
            {
                //Debug.Log(androidPath);
                string args = "sdk";
                ProcessStartInfo ps = new ProcessStartInfo(androidPath, args);
                Process p = new Process();
                p.StartInfo = ps;
                p.Exited += (object sender, EventArgs e) =>
                                {
                                    p.Dispose();
                                };
                p.Start();
            }
        }
    }

    static bool GenerateRJava()
    {
        //clean meta files
        List<string> files = new List<string>();
        GetAssets("*.meta", files, new DirectoryInfo(pathRes.Replace(@"\", "/")));

        foreach (string meta in files)
        {
            //Debug.Log(meta);
            if (File.Exists(meta))
            {
                File.Delete(meta);
            }
        }

        if (!Directory.Exists(pathBin))
        {
            Directory.CreateDirectory(pathBin);
        }

        if (!Directory.Exists(pathBin))
        {
            Directory.CreateDirectory(pathBin);
        }

        if (Directory.Exists(pathBin))
        {
            Debug.Log(string.Format("Path exists: {0}", pathBin));
        }
        else
        {
            Debug.Log(string.Format("Path not exists: {0}", pathBin));
            return false;
        }

        Thread.Sleep(100);

        RunProcess(pathAAPT, string.Format("package -v -f -m -J gen -M \"{0}\" -S \"{1}\" -I \"{2}\" -F \"{3}/resources.ap_\" -J \"{4}\"",
            pathManifestPath, pathRes, GetPathAndroidJar(), pathBin, pathSrc));

        string pathRJava = GetRJava();
        if (string.IsNullOrEmpty(pathRJava))
        {
            Debug.LogError("Path to R.java is empty");
            return false;
        }
        if (File.Exists(pathRJava))
        {
            using (FileStream fs = File.Open(pathRJava, FileMode.Open, FileAccess.Read, FileShare.ReadWrite))
            {
                using (StreamReader sr = new StreamReader(fs))
                {
                    Debug.Log(sr.ReadToEnd());
                }
            }
        }

        if (Directory.Exists(pathBin))
        {
            Directory.Delete(pathBin, true);
        }

        return true;
    }

    static bool CompileApplicationClasses()
    {
        string pathAndroid = string.Format("{0}/Assets/Plugins/Android", pathUnityProject);
        if (!Directory.Exists(pathAndroid))
        {
            Debug.LogError(string.Format("Failed to find Android Path: {0}", pathAndroid));
            return false;
        }

        string pathClasses = string.Format("{0}/Assets/Plugins/Android/Classes", pathUnityProject);
        if (!Directory.Exists(pathClasses))
        {
            Directory.CreateDirectory(pathClasses);
        }
        string pathRJava = GetRJava();
        if (string.IsNullOrEmpty(pathRJava))
        {
            Debug.LogError("Path to R.java is empty");
            return false;
        }
        if (!File.Exists(pathRJava))
        {
            Debug.LogError("R.java cannot be found");
            return false;
        }
        string includeFiles = string.Format("\"{0}/{1}.java\" \"{0}/IOuyaActivity.java\" \"{0}/TestOuyaFacade.java\" \"{2}\"",
            pathSrc, javaAppName, pathRJava);
        string jars = string.Empty;

        if (File.Exists(pathToolsJar))
        {
            Debug.Log(string.Format("Found Java tools jar: {0}", pathToolsJar));
        }
        else
        {
            Debug.LogError(string.Format("Failed to find Java tools jar: {0}", pathToolsJar));
            return false;
        }

        if (File.Exists(GetPathAndroidJar()))
        {
            Debug.Log(string.Format("Found Android jar: {0}", GetPathAndroidJar()));
        }
        else
        {
            Debug.LogError(string.Format("Failed to find Android jar: {0}", GetPathAndroidJar()));
            return false;
        }

        if (File.Exists(pathGsonJar))
        {
            Debug.Log(string.Format("Found GJON jar: {0}", pathGsonJar));
        }
        else
        {
            Debug.LogError(string.Format("Failed to find GSON jar: {0}", pathGsonJar));
            return false;
        }

        if (File.Exists(pathUnityJar))
        {
            Debug.Log(string.Format("Found Unity jar: {0}", pathUnityJar));
        }
        else
        {
            Debug.LogError(string.Format("Failed to find Unity jar: {0}", pathUnityJar));
            return false;
        }

        if (File.Exists(pathOuyaUnityPluginJar))
        {
            Debug.Log(string.Format("Found Ouya Unity Plugin jar: {0}", pathOuyaUnityPluginJar));
        }
        else
        {
            Debug.LogError(string.Format("Failed to find Ouya Unity Plugin jar: {0}", pathOuyaUnityPluginJar));
            return false;
        }

        if (File.Exists(pathOuyaSDKJar))
        {
            Debug.Log(string.Format("Found Ouya SDK jar: {0}", pathOuyaSDKJar));
        }
        else
        {
            Debug.LogError(string.Format("Failed to find Ouya SDK jar: {0}", pathOuyaSDKJar));
            return false;
        }

        string output = string.Empty;
        string error = string.Empty;

        switch (Application.platform)
        {
            case RuntimePlatform.OSXEditor:
                jars = string.Format("\"{0}:{1}:{2}:{3}:{4}:{5}\"",
                    pathToolsJar, GetPathAndroidJar(), pathGsonJar, pathUnityJar, pathOuyaUnityPluginJar, pathOuyaSDKJar);

                RunProcess(pathJavaC, string.Empty, string.Format("-g -source 1.6 -target 1.6 {0} -classpath {1} -bootclasspath {1} -d \"{2}\"",
                    includeFiles,
                    jars,
                    pathClasses),
                    ref output,
                    ref error);
                break;
            case RuntimePlatform.WindowsEditor:
                jars = string.Format("\"{0}\";\"{1}\";\"{2}\";\"{3}\";\"{4}\";\"{5}\"",
                    pathToolsJar, GetPathAndroidJar(), pathGsonJar, pathUnityJar, pathOuyaUnityPluginJar, pathOuyaSDKJar);

                RunProcess(pathJavaC, string.Empty, string.Format("-Xlint:deprecation -g -source 1.6 -target 1.6 {0} -classpath {1} -bootclasspath {1} -d \"{2}\"",
                    includeFiles,
                    jars,
                    pathClasses),
                    ref output,
                    ref error);
                break;
        }

        if (!string.IsNullOrEmpty(error))
        {
            Debug.LogError(error);
            return false;
        }

        return true;
    }

    static bool BuildApplicationJar()
    {
        string pathClasses = string.Format("{0}/Assets/Plugins/Android/Classes", pathUnityProject);
        string bundlePrefix = GetBundlePrefix();
        if (string.IsNullOrEmpty(bundlePrefix))
        {
            Debug.LogError("Bundle prefix cannot be empty");
            return false;
        }

        //@hack: remove extra class file
        // tv/ouya/sdk/IOuyaActivity.class
        string extraClass = string.Format("{0}\\tv\\ouya\\sdk\\IOuyaActivity.class", pathClasses);
        if (File.Exists(extraClass))
        {
            File.Delete(extraClass);
            Debug.Log(string.Format("Removed: {0}", extraClass));
        }
        extraClass = string.Format("{0}\\tv\\ouya\\sdk\\IOuyaActivity.class.meta", pathClasses);
        if (File.Exists(extraClass))
        {
            File.Delete(extraClass);
            Debug.Log(string.Format("Removed: {0}", extraClass));
        }

        //@hack: remove extra class file
        // tv/ouya/sdk/TestOuyaFacade.class
        extraClass = string.Format("{0}\\tv\\ouya\\sdk\\TestOuyaFacade.class", pathClasses);
        if (File.Exists(extraClass))
        {
            File.Delete(extraClass);
            Debug.Log(string.Format("Removed: {0}", extraClass));
        }
        extraClass = string.Format("{0}\\tv\\ouya\\sdk\\TestOuyaFacade.class.meta", pathClasses);
        if (File.Exists(extraClass))
        {
            File.Delete(extraClass);
            Debug.Log(string.Format("Removed: {0}", extraClass));
        }

        //@hack: remove extra folder
        // tv/ouya/sdk/*
        string extraFolder = string.Format("{0}\\tv\\ouya\\sdk", pathClasses);
        if (Directory.Exists(extraFolder))
        {
            Directory.Delete(extraFolder, true);
            Debug.Log(string.Format("Removed: {0}", extraFolder));
        }
        string extraMeta = string.Format("{0}\\tv\\ouya\\sdk.meta", pathClasses);
        if (File.Exists(extraMeta))
        {
            File.Delete(extraMeta);
            Debug.Log(string.Format("Removed: {0}", extraMeta));
        }

        RunProcess(pathJar, pathClasses, string.Format("cvfM OuyaUnityApplication.jar {0}/", bundlePrefix));
        OuyaPanel.RunProcess(pathJavaP, pathClasses, string.Format("-s {0}.{1}", PlayerSettings.bundleIdentifier, javaAppName));
        string pathAppJar = string.Format("{0}/OuyaUnityApplication.jar", pathClasses);
        string pathDest = string.Format("{0}/Assets/Plugins/Android/OuyaUnityApplication.jar", pathUnityProject);
        try
        {
            if (File.Exists(pathDest))
            {
                File.Delete(pathDest);
            }
            if (File.Exists(pathAppJar))
            {
                File.Move(pathAppJar, pathDest);
            }
            if (Directory.Exists(pathClasses))
            {
                Directory.Delete(pathClasses, true);
            }

            extraMeta = string.Format("{0}.meta", pathClasses);
            if (File.Exists(extraMeta))
            {
                File.Delete(extraMeta);
                Debug.Log(string.Format("Removed: {0}", extraMeta));
            }
        }
        catch (System.Exception)
        {

        }

        return true;
    }

    public static bool CompileApplicationJava()
    {
        if (GenerateRJava() &&
            CompileApplicationClasses() &&
            BuildApplicationJar())
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    void GUIDisplayFolder(string label, string path)
    {
        bool dirExists = Directory.Exists(path);

        if (!dirExists)
        {
            GUI.enabled = false;
        }
        GUILayout.BeginHorizontal(GUILayout.MaxWidth(position.width));
        GUILayout.Space(25);
        GUILayout.Label(string.Format("{0}:", label), GUILayout.Width(100));
        GUILayout.Space(5);
        GUILayout.Label(path.Replace("/", @"\"), EditorStyles.wordWrappedLabel, GUILayout.MaxWidth(position.width - 130));
        GUILayout.EndHorizontal();
        if (!dirExists)
        {
            GUI.enabled = true;
        }
    }

    void GUIDisplayFile(string label, string path)
    {
        bool fileExists = File.Exists(path);

        if (!fileExists)
        {
            GUI.enabled = false;
        }
        GUILayout.BeginHorizontal(GUILayout.MaxWidth(position.width));
        GUILayout.Space(25);
        GUILayout.Label(string.Format("{0}:", label), GUILayout.Width(100));
        GUILayout.Space(5);
        GUILayout.Label(path.Replace("/", @"\"), EditorStyles.wordWrappedLabel, GUILayout.MaxWidth(position.width - 130));
        GUILayout.EndHorizontal();
        if (!fileExists)
        {
            GUI.enabled = true;
        }
    }

    void GUIDisplayUnityFile(string label, string path)
    {
        bool fileExists = File.Exists(path);

        if (!fileExists)
        {
            GUI.enabled = false;
        }
        GUILayout.BeginHorizontal(GUILayout.MaxWidth(position.width));
        GUILayout.Space(25);
        GUILayout.Label(string.Format("{0}:", label), GUILayout.Width(100));
        GUILayout.Space(5);
        if (string.IsNullOrEmpty(path))
        {
            EditorGUILayout.ObjectField(string.Empty, null, typeof(UnityEngine.Object), false);
        }
        else
        {
            try
            {
                DirectoryInfo assets = new DirectoryInfo("Assets");
                Uri assetsUri = new Uri(assets.FullName);
                FileInfo fi = new FileInfo(path);
                string relativePath = assetsUri.MakeRelativeUri(new Uri(fi.FullName)).ToString();
                UnityEngine.Object fileRef = AssetDatabase.LoadAssetAtPath(relativePath, typeof(UnityEngine.Object));
                EditorGUILayout.ObjectField(string.Empty, fileRef, typeof(UnityEngine.Object), false);
            }
            catch (System.Exception)
            {
                Debug.LogError(string.Format("Path is invalid: label={0} path={1}", label, path));
            }
        }
        GUILayout.EndHorizontal();
        if (!fileExists)
        {
            GUI.enabled = true;
        }
    }

    // load the scene in the update method
    private string m_nextScene = string.Empty;

    private Vector2 m_scroll = Vector2.zero;

    void OnGUI()
    {
        m_scroll = GUILayout.BeginScrollView(m_scroll, GUILayout.MaxWidth(position.width));

        GUILayout.Label(string.Format("{0} UID: {1}", OuyaSDK.VERSION, UID));

        m_selectedToolSet = GUILayout.Toolbar(m_selectedToolSet, m_toolSets, GUILayout.MaxWidth(position.width));

        GUILayout.Space(20);

        switch (m_selectedToolSet)
        {
            case 0:

                if (GUILayout.Button("Run Application"))
                {
                    m_toggleRunApplication = true;
                }

                if (GUILayout.Button("Build Application"))
                {
                    m_toggleCompileJava = true;
                    m_toggleCompilePlugin = true;
                    m_toggleCompileNDK = true;
                    m_toggleBuildApplication = true;
                }

                if (GUILayout.Button("Build and Run Application"))
                {
                    m_toggleBuildAndRunApplication = true;
                }

                if (GUILayout.Button("Build, Run, and Compile Application"))
                {
                    m_toggleBuildRunAndCompileApplication = true;
                }

                if (GUILayout.Button("Compile"))
                {
                    m_toggleCompileJava = true;
                    m_toggleCompileNDK = true;
                }

                if (GUILayout.Button("Compile Java"))
                {
                    m_toggleCompileJava = true;
                }

                if (GUILayout.Button("Compile Plugin"))
                {
                    m_toggleCompilePlugin = true;
                }

                if (GUILayout.Button("Compile NDK"))
                {
                    m_toggleCompileNDK = true;
                }

                /*
                if (GUILayout.Button("Build Application Jar"))
                {
                    BuildApplicationJar();
                }

                if (GUILayout.Button("Compile Application Classes"))
                {
                    CompileApplicationClasses();
                }

                if (GUILayout.Button("Generate R.java from main layout"))
                {
                    GenerateRJava();
                }
                */

                UsePostProcessor = GUILayout.Toggle(UsePostProcessor, "Enable Java/C++ post processor");

                GUILayout.Space(5);
                if (GUILayout.Button("Sync Bundle ID"))
                {
                    m_toggleSyncBundleID = true;
                }
                GUILayout.Space(5);

                #region Example scenes

                GUILayout.Label("Build Settings:");

                if (GUILayout.Button("Use SetResolution Scene"))
                {
                    EditorBuildSettingsScene[] scenes =
                    {
                        new EditorBuildSettingsScene("Assets/Ouya/Examples/Scenes/SceneSetResolution.unity", true), 
                    };
                    EditorBuildSettings.scenes = scenes;
                    m_nextScene = scenes[0].path;
                }

                if (GUILayout.Button("Use SceneMultipleControllers Scene"))
                {
                    EditorBuildSettingsScene[] scenes =
                    {
                        new EditorBuildSettingsScene("Assets/Ouya/Examples/Scenes/SceneMultipleControllers.unity", true), 
                    };
                    EditorBuildSettings.scenes = scenes;
                    m_nextScene = scenes[0].path;
                }

                if (GUILayout.Button("Use ShowController Scene"))
                {
                    EditorBuildSettingsScene[] scenes =
                    {
                        new EditorBuildSettingsScene("Assets/Ouya/Examples/Scenes/SceneShowController.unity", true), 
                    };
                    EditorBuildSettings.scenes = scenes;
                    m_nextScene = scenes[0].path;
                }

                if (GUILayout.Button("Use ShowUnityInput Scene"))
                {
                    EditorBuildSettingsScene[] scenes =
                    {
                        new EditorBuildSettingsScene("Assets/Ouya/Examples/Scenes/SceneShowUnityInput.unity", true), 
                    };
                    EditorBuildSettings.scenes = scenes;
                    m_nextScene = scenes[0].path;
                }

                if (GUILayout.Button("Use SceneShowNDK Scene"))
                {
                    EditorBuildSettingsScene[] scenes =
                    {
                        new EditorBuildSettingsScene("Assets/Ouya/Examples/Scenes/SceneShowNDK.unity", true), 
                    };
                    EditorBuildSettings.scenes = scenes;
                    m_nextScene = scenes[0].path;
                }

                if (GUILayout.Button("Use SceneShowProducts Scene"))
                {
                    EditorBuildSettingsScene[] scenes =
                    {
                        new EditorBuildSettingsScene("Assets/Ouya/Examples/Scenes/SceneShowProducts.unity", true), 
                    };
                    EditorBuildSettings.scenes = scenes;
                    m_nextScene = scenes[0].path;
                }

                if (GUILayout.Button("Use Starter Kit Scenes"))
                {
                    EditorBuildSettingsScene[] scenes =
                    {
                        new EditorBuildSettingsScene("Assets/Ouya/StarterKit/Scenes/SceneInit.unity", true), 
                        new EditorBuildSettingsScene("Assets/Ouya/StarterKit/Scenes/SceneSplash.unity", true),
                        new EditorBuildSettingsScene("Assets/Ouya/StarterKit/Scenes/SceneMain.unity", true),
                        new EditorBuildSettingsScene("Assets/Ouya/StarterKit/Scenes/SceneGame.unity", true),
                    };
                    EditorBuildSettings.scenes = scenes;
                    m_nextScene = scenes[0].path;
                }

                #endregion

                GUILayout.Label("OUYA", EditorStyles.boldLabel);

                GUILayout.BeginHorizontal(GUILayout.MaxWidth(position.width));
                GUILayout.Space(25);
                GUILayout.Label("Bundle Identifier", GUILayout.Width(100));
                GUILayout.Space(5);
                PlayerSettings.bundleIdentifier = GUILayout.TextField(PlayerSettings.bundleIdentifier, EditorStyles.wordWrappedLabel, GUILayout.MaxWidth(position.width - 130));
                GUILayout.EndHorizontal();

                GUILayout.BeginHorizontal(GUILayout.MaxWidth(position.width));
                GUILayout.Space(25);
                GUILayout.Label("APK Name:", GUILayout.Width(100));
                GUILayout.Space(5);
                string newApkName = GUILayout.TextField(apkName, EditorStyles.wordWrappedLabel, GUILayout.MaxWidth(position.width - 130));
                GUILayout.EndHorizontal();
                if (apkName != newApkName)
                {
                    apkName = newApkName;
                    EditorPrefs.SetString(KEY_APK_NAME, apkName);
                }

                string error = string.Empty;
                GUILayout.BeginHorizontal(GUILayout.MaxWidth(position.width));
                GUILayout.Space(25);
                GUILayout.Label("Java App Class:", GUILayout.Width(100));
                GUILayout.Space(5);
                if (!string.IsNullOrEmpty(error))
                {
                    GUILayout.Label(error);
                }
                string newJavaAppName = GUILayout.TextField(javaAppName, EditorStyles.wordWrappedLabel, GUILayout.MaxWidth(position.width - 130));
                GUILayout.EndHorizontal();
                if (javaAppName != newJavaAppName)
                {
                    javaAppName = newJavaAppName;
                    EditorPrefs.SetString(KEY_JAVA_APP_NAME, javaAppName);
                }

                GUILayout.BeginHorizontal(GUILayout.MaxWidth(position.width));
                GUILayout.Space(25);
                GUILayout.Label(string.Format("{0}:", "Bundle Prefix"), GUILayout.Width(100));
                GUILayout.Space(5);
                GUILayout.Label(GetBundlePrefix(), EditorStyles.wordWrappedLabel, GUILayout.MaxWidth(position.width - 130));
                GUILayout.EndHorizontal();

                string javaPackageName = GetApplicationJavaPackageName();
                if (!javaPackageName.Equals(string.Format("package {0};", PlayerSettings.bundleIdentifier)))
                {
                    error = "[error] (bundle mismatched)\n";
                }
                else
                {
                    error = string.Empty;
                }
                GUILayout.BeginHorizontal(GUILayout.MaxWidth(position.width));
                GUILayout.Space(25);
                GUILayout.Label("App Java Pack:", GUILayout.Width(100));
                GUILayout.Space(5);
                GUILayout.Label(string.Format("{0}{1}", error, GetApplicationJavaPackageName()), EditorStyles.wordWrappedLabel, GUILayout.MaxWidth(position.width - 130));
                GUILayout.EndHorizontal();

                string manifestPackageName = GetAndroidManifestPackageName();
                if (!manifestPackageName.Equals(PlayerSettings.bundleIdentifier))
                {
                    error = "[error] (bundle mismatched)\n";
                }
                else
                {
                    error = string.Empty;
                }
                GUILayout.BeginHorizontal(GUILayout.MaxWidth(position.width));
                GUILayout.Space(25);
                GUILayout.Label("Manifest Pack:", GUILayout.Width(100));
                GUILayout.Space(5);
                GUILayout.Label(string.Format("{0}{1}", error, GetAndroidManifestPackageName()), EditorStyles.wordWrappedLabel, GUILayout.MaxWidth(position.width - 130));
                GUILayout.EndHorizontal();

                GameObject go = GameObject.Find("OuyaGameObject");
                OuyaGameObject ouyaGO = null;
                if (go)
                {
                    ouyaGO = go.GetComponent<OuyaGameObject>();
                }
                if (null == ouyaGO)
                {
                    GUI.enabled = false;
                }
                GUILayout.BeginHorizontal(GUILayout.MaxWidth(position.width));
                GUILayout.Space(25);
                GUILayout.Label(string.Format("{0}:", "GameObject"), GUILayout.Width(100));
                GUILayout.Space(5);
                EditorGUILayout.ObjectField(string.Empty, ouyaGO, typeof(OuyaGameObject), true);
                GUILayout.EndHorizontal();
                if (null == ouyaGO)
                {
                    GUI.enabled = true;
                }

                GUIDisplayUnityFile(KEY_PATH_OUYA_SDK, pathOuyaSDKJar);
                GUIDisplayUnityFile(KEY_PATH_JAR_GUAVA, pathGsonJar);
                GUIDisplayUnityFile(KEY_PATH_JAR_GSON, pathGuavaJar);
                GUIDisplayUnityFile(KEY_PATH_JAR_OUYA_UNITY_PLUGIN, pathOuyaUnityPluginJar);
                GUIDisplayUnityFile("Manifest", pathManifestPath);
                GUIDisplayUnityFile("R.Java", GetRJava());
                GUIDisplayUnityFile("Application.Java", GetApplicationJava());
                GUIDisplayUnityFile("IOuyaActivity.Java", GetIOuyaActivityJava());
                //GUIDisplayFolder("Bin", pathBin);
                GUIDisplayFolder("Res", pathRes);
                GUIDisplayFolder("Src", pathSrc);

                if (GUILayout.Button("Subscribe for plugin updates"))
                {
                    Application.OpenURL("https://hashbanggamesportal.ontimenow.com/");
                }

                if (GUILayout.Button("Check for plugin updates"))
                {
                    Application.OpenURL("http://tagenigma.com/ouya/OuyaSDK-1.0.0/");
                }

                if (GUILayout.Button("Read OUYA Unity Package Doc"))
                {
                    Application.OpenURL("http://tagenigma.com/ouya/OuyaSDK-1.0.0/OUYA_Unity_Package.pdf");
                }

                if (GUILayout.Button("Visit Unity3d on OUYA Forum"))
                {
                    Application.OpenURL("http://forums.ouya.tv/categories/unity-on-ouya");
                }

                if (GUILayout.Button("OUYA Developer Portal"))
                {
                    Application.OpenURL("https://devs.ouya.tv/developers");
                }

                break;
            case 1:
                GUILayout.Label("Unity Paths", EditorStyles.boldLabel);

                GUIDisplayFile(KEY_PATH_UNITY_JAR, pathUnityJar);
                GUIDisplayFolder(KEY_PATH_UNITY_EDITOR, pathUnityEditor);
                GUIDisplayFolder(KEY_PATH_UNITY_PROJECT, pathUnityProject);

                if (GUILayout.Button("Download Unity3d"))
                {
                    Application.OpenURL("http://unity3d.com/unity/download/");
                }

                if (GUILayout.Button("Unity3d Training"))
                {
                    Application.OpenURL("http://unity3d.com/learn");
                }

                if (GUILayout.Button("Unity3d Scripting Reference"))
                {
                    Application.OpenURL("http://docs.unity3d.com/Documentation/ScriptReference/index.html");
                }

                break;
            case 2:
                GUILayout.Label("Java JDK Paths", EditorStyles.boldLabel);

                GUIDisplayFile(KEY_PATH_JAVA_TOOLS_JAR, pathToolsJar);
                GUIDisplayFile(KEY_PATH_JAVA_JAR, pathJar);
                GUIDisplayFile(KEY_PATH_JAVA_JAVAC, pathJavaC);
                GUIDisplayFile(KEY_PATH_JAVA_JAVAP, pathJavaP);
                GUIDisplayFolder(KEY_PATH_JAVA_JDK, pathJDK);

                GUILayout.BeginHorizontal(GUILayout.MaxWidth(position.width));
                if (GUILayout.Button("Select SDK Path..."))
                {
                    SelectJavaJDKPaths();
                }
                if (GUILayout.Button("Reset Paths"))
                {
                    ResetJavaJDKPaths();
                }

                GUILayout.EndHorizontal();

                if (GUILayout.Button("Download JDK 6 32-bit"))
                {
                    Application.OpenURL("http://www.oracle.com/technetwork/java/javase/downloads/jdk6downloads-1902814.html");
                }

                break;
            case 3:
                GUILayout.Label("Android SDK", EditorStyles.boldLabel);

                GUILayout.BeginHorizontal(GUILayout.MaxWidth(position.width));
                GUILayout.Space(25);
                GUILayout.Label(string.Format("{0}:", "minSDKVersion"), GUILayout.Width(100));
                GUILayout.Space(5);
                GUILayout.Label(((int)(PlayerSettings.Android.minSdkVersion)).ToString(), EditorStyles.wordWrappedLabel, GUILayout.MaxWidth(position.width - 130));
                GUILayout.EndHorizontal();

                GUIDisplayFile(KEY_PATH_ANDROID_JAR, GetPathAndroidJar());
                GUIDisplayFile(KEY_PATH_ANDROID_ADB, pathADB);
                GUIDisplayFile(KEY_PATH_ANDROID_AAPT, pathAAPT);
                GUIDisplayFolder(KEY_PATH_ANDROID_SDK, pathSDK);

                GUILayout.BeginHorizontal(GUILayout.MaxWidth(position.width));
                if (GUILayout.Button("Select SDK Path..."))
                {
                    SelectAndroidSDKPaths();
                }
                if (GUILayout.Button("Reset Paths"))
                {
                    ResetAndroidSDKPaths();
                }
                GUILayout.EndHorizontal();

                if (GUILayout.Button("Download Android SDK"))
                {
                    Application.OpenURL("http://developer.android.com/sdk/index.html");
                }

                if (GUILayout.Button("Open Android SDK"))
                {
                    m_toggleOpenAndroidSDK = true;
                }

                if (GUILayout.Button("Open Shell"))
                {
                    string shellPath = @"c:\windows\system32\cmd.exe";
                    if (File.Exists(shellPath))
                    {
                        //Debug.Log(appPath);
                        //Debug.Log(pathADB);
                        string args = string.Format(@"/k");
                        //Debug.Log(args);
                        ProcessStartInfo ps = new ProcessStartInfo(shellPath, args);
                        Process p = new Process();
                        ps.RedirectStandardOutput = false;
                        ps.UseShellExecute = true;
                        ps.CreateNoWindow = false;
                        ps.WorkingDirectory = Path.GetDirectoryName(pathADB);
                        p.StartInfo = ps;
                        p.Exited += (object sender, EventArgs e) =>
                                        {
                                            p.Dispose();
                                        };
                        p.Start();
                        //p.WaitForExit();
                    }
                    EditorGUIUtility.ExitGUI();
                }

                if (GUILayout.Button("Open Logcat"))
                {
                    if (File.Exists(pathADB))
                    {
                        //Debug.Log(appPath);
                        //Debug.Log(pathADB);
                        string args = string.Format(@"shell logcat");
                        //Debug.Log(args);
                        ProcessStartInfo ps = new ProcessStartInfo(pathADB, args);
                        Process p = new Process();
                        ps.RedirectStandardOutput = false;
                        ps.UseShellExecute = true;
                        ps.CreateNoWindow = false;
                        ps.WorkingDirectory = Path.GetDirectoryName(pathADB);
                        p.StartInfo = ps;
                        p.Exited += (object sender, EventArgs e) =>
                        {
                            p.Dispose();
                        };
                        p.Start();
                        //p.WaitForExit();
                    }
                    EditorGUIUtility.ExitGUI();
                }

                break;

            case 4:
                GUILayout.Label("Android NDK", EditorStyles.boldLabel);

                GUIDisplayFolder(KEY_PATH_ANDROID_NDK, pathNDK);
                GUIDisplayFile(KEY_PATH_ANDROID_NDK_MAKE, pathNDKMake);
                GUIDisplayUnityFile(KEY_PATH_ANDROID_JNI_CPP, pathJNICpp);
                GUIDisplayUnityFile(KEY_PATH_ANDROID_JNI_MK, pathJNIMk);
                GUIDisplayUnityFile(KEY_PATH_OUYA_NDK_LIB, pathOuyaNDKLib);

                GUILayout.BeginHorizontal(GUILayout.MaxWidth(position.width));
                if (GUILayout.Button("Select NDK Path..."))
                {
                    SelectAndroidNDKPaths();
                }
                if (GUILayout.Button("Reset Paths"))
                {
                    ResetAndroidNDKPaths();
                }
                GUILayout.EndHorizontal();

                if (GUILayout.Button("Download Android NDK"))
                {
                    Application.OpenURL("http://developer.android.com/tools/sdk/ndk/index.html");
                }

                break;
        }

        GUILayout.EndScrollView();
    }

    #region RUN PROCESS
    public static void RunProcess(string path, string arguments)
    {
        string error = string.Empty;
        string output = string.Empty;
        RunProcess(path, string.Empty, arguments, ref output, ref error);
    }

    public static void RunProcess(string path, string workingDirectory, string arguments)
    {
        string error = string.Empty;
        string output = string.Empty;
        RunProcess(path, workingDirectory, arguments, ref output, ref error);
    }

    public static void RunProcess(string path, string workingDirectory, string arguments, ref string output, ref string error)
    {
        try
        {
            System.Diagnostics.Process process = new System.Diagnostics.Process();
            process.StartInfo.Arguments = arguments;
            process.StartInfo.WorkingDirectory = workingDirectory;
            process.StartInfo.FileName = path;
            process.StartInfo.UseShellExecute = false;
            process.StartInfo.ErrorDialog = true;
            process.StartInfo.RedirectStandardOutput = true;
            process.StartInfo.RedirectStandardInput = true;
            process.StartInfo.RedirectStandardError = true;
            DateTime startTime = DateTime.Now;
            Debug.Log(string.Format("[Running Process] filename={0} arguments={1}", process.StartInfo.FileName,
                                    process.StartInfo.Arguments));

            process.Start();

            output = process.StandardOutput.ReadToEnd();
            error = process.StandardError.ReadToEnd();

            float elapsed = (float)(DateTime.Now - startTime).TotalSeconds;
            Debug.Log(string.Format("[Results] elapsedTime: {3} errors: {2}\noutput: {1}", process.StartInfo.FileName,
                                    output, error, elapsed));

            //if (output.Length > 0 ) Debug.Log("Output: " + output);
            //if (error.Length > 0 ) Debug.Log("Error: " + error); 
        }
        catch (System.Exception ex)
        {
            Debug.LogWarning(string.Format("Unable to run process: path={0} arguments={1} exception={2}", path, arguments, ex));
        }

    }
    #endregion

    #region File IO

    private static void GetAssets(string extension, List<string> files, DirectoryInfo directory)
    {
        if (null == directory)
        {
            return;
        }
        foreach (FileInfo file in directory.GetFiles(extension))
        {
            if (string.IsNullOrEmpty(file.FullName) ||
                files.Contains(file.FullName))
            {
                continue;
            }
            files.Add(file.FullName);
            //Debug.Log(string.Format("File: {0}", file.FullName));
        }
        foreach (DirectoryInfo subDir in directory.GetDirectories())
        {
            if (null == subDir)
            {
                continue;
            }
            //Debug.Log(string.Format("Directory: {0}", subDir));
            GetAssets(extension, files, subDir);
        }
    }

    #endregion

    #region Unique identification

    public static string UID = GetUID();

    /// <summary>
    /// Get the machine name
    /// </summary>
    /// <returns></returns>
    private static string GetMachineName()
    {
        try
        {
            string machineName = System.Environment.MachineName;
            if (!string.IsNullOrEmpty(machineName))
            {
                return machineName;
            }
        }
        catch (System.Exception)
        {
            Debug.LogError("GetMachineName: Failed to get machine name");
        }

        return "Unknown";
    }

    /// <summary>
    /// Get the IDE process IDE
    /// </summary>
    /// <returns></returns>
    private static int GetProcessID()
    {
        try
        {
            Process process = Process.GetCurrentProcess();
            if (null != process)
            {
                return process.Id;
            }
        }
        catch
        {
            Debug.LogError("GetProcessID: Failed to get process id");
        }

        return 0;
    }

    /// <summary>
    /// Get a unique identifier for the Unity instance
    /// </summary>
    /// <returns></returns>
    private static string GetUID()
    {
        try
        {
            return string.Format("{0}_{1}", GetMachineName(), GetProcessID());
        }
        catch (System.Exception)
        {
            Debug.LogError("GetUID: Failed to create uid");
        }

        return string.Empty;
    }

    #endregion
}
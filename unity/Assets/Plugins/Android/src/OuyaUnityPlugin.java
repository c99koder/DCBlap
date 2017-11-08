package tv.ouya.sdk;

import android.app.Activity;
import android.content.Intent;
import android.util.Log;
import android.widget.FrameLayout;

import java.io.File;

import com.unity3d.player.UnityPlayer;

import tv.ouya.console.api.*;

public class OuyaUnityPlugin
{
	// The plugin has an instance of the OuyaFacade
	private static TestOuyaFacade m_test = null;

	// Unity sets the developer id
	// Unity maybe set the iap test mode
	private static Boolean m_unityInitialized = false;
	public static Boolean isUnityInitialized()
	{
		return m_unityInitialized;
	}
	public static void unityInitialized()
	{
		m_unityInitialized = true;

		//if IAP is enabled initialize the ouya facade
		if (m_enableIAP)
		{
			InitializeTest();
		}
	}

	// java needs to tell unity that the test facade has been
	// initialized
	private static Boolean m_pluginAwake = false;
	public static Boolean isPluginAwake()
	{
		return m_pluginAwake;
	}

	// the developer id is sent from Unity
	private static String m_developerId = "";

	// ENABLE IAP mode this is for testing/debugging to turn off IAP
	private static Boolean m_enableIAP = true;

	// Legacy input is for debugging which sends verbose JSON data
	private static Boolean m_useLegacyInput = false;

	// For debugging enable logging for testing
	private static Boolean m_enableDebugLogging = true;

	// This initializes the Unity plugin - our OuyaUnityPlugin,
	// and it gets a generic reference to the activity
	public OuyaUnityPlugin(Activity currentActivity)
	{
		Log.i("Unity", "OuyaUnityPlugin: Plugin is awake");
		
		m_pluginAwake = true;
	}

	public static Boolean getUseLegacyInput()
	{
		return m_useLegacyInput;
	}

	public static void useLegacyInput()
	{
		m_useLegacyInput = true;
	}

	public static void setResolution(String resolutionId)
	{
		if (null == IOuyaActivity.GetUnityPlayer())
		{
			Log.i("Unity", "IOuyaActivity.GetUnityPlayer() is null");
			return;
		}
		if (resolutionId.equals("640x480"))
		{
			if (null != IOuyaActivity.GetLayout())
			{
				IOuyaActivity.GetActivity().runOnUiThread(new Runnable() {
					@Override
					public void run() {
						Log.i("Unity", "IOuyaActivity.GetLayout().layout(0, 0, 640, 480);");
						IOuyaActivity.GetLayout().getLayoutParams().width = 640;
						IOuyaActivity.GetLayout().getLayoutParams().height = 480;
						IOuyaActivity.GetLayout().requestLayout();
					}
				});
			}
		}
		else if (resolutionId.equals("1280x720"))
		{
			if (null != IOuyaActivity.GetLayout())
			{
				IOuyaActivity.GetActivity().runOnUiThread(new Runnable() {
					@Override
					public void run() {
						Log.i("Unity", "IOuyaActivity.GetLayout().layout(0, 0, 1280, 720);");
						IOuyaActivity.GetLayout().getLayoutParams().width = 1280;
						IOuyaActivity.GetLayout().getLayoutParams().height = 720;
						IOuyaActivity.GetLayout().requestLayout();
					}
				});
			}
		}
		else if (resolutionId.equals("1920x1080"))
		{
			if (null != IOuyaActivity.GetLayout())
			{
				IOuyaActivity.GetActivity().runOnUiThread(new Runnable() {
					@Override
					public void run() {
						Log.i("Unity", "IOuyaActivity.GetLayout().layout(0, 0, 1920, 1080);");
						IOuyaActivity.GetLayout().getLayoutParams().width = 1920;
						IOuyaActivity.GetLayout().getLayoutParams().height = 1080;
						IOuyaActivity.GetLayout().requestLayout();
					}
				});
			}
		}
	}

	// most of the java functions that are called, need the ouya facade initialized
	private static void InitializeTest()
	{
		try		
		{
			// check if the ouya facade is constructed
			if (null == m_test)
			{
				if (m_enableDebugLogging)
				{
					Log.i("Unity", "InitializeTest: m_test is null");
				}
			}

			if (null == IOuyaActivity.GetActivity())
			{
				if (m_enableDebugLogging)
				{
					Log.i("Unity", "InitializeTest: IOuyaActivity.GetActivity() is null");
				}
				return;
			}

			if (null == m_test)
			{
				//wait for Unity to initialize
				if (!m_unityInitialized)
				{
					if (m_enableDebugLogging)
					{
						if (m_developerId.equals(""))
						{
							Log.i("Unity", "InitializeTest: m_developerId is empty, requesting init");
						}
						else
						{
							Log.i("Unity", "InitializeTest: m_developerId is set, requesting init");
						}
					}

					UnityPlayer.UnitySendMessage("OuyaGameObject", "RequestUnityAwake", null);
				}
				else
				{
					if (m_enableDebugLogging)
					{
						Log.i("Unity", "InitializeTest: Unity has initialized,  constructing TestOuyaFacade");
					}

					/*
					if (null == IOuyaActivity.GetSavedInstanceState())
					{
						Log.i("Unity", "InitializeTest: IOuyaActivity.GetSavedInstanceState() == null");
					}
					else
					{
						Log.i("Unity", "InitializeTest: m_developerId is valid,  constructing TestOuyaFacade");
						m_test = new TestOuyaFacade(IOuyaActivity.GetActivity(), IOuyaActivity.GetSavedInstanceState(), m_developerId);
						IOuyaActivity.SetTestOuyaFacade(m_test);
					}
					*/
					
					m_test = new TestOuyaFacade(IOuyaActivity.GetActivity(), IOuyaActivity.GetSavedInstanceState(), m_developerId);
					
					//make facade accessible by activity
					IOuyaActivity.SetTestOuyaFacade(m_test);
				}
			}
		}
		catch (Exception ex) 
		{
			Log.i("Unity", "InitializeTest: InitializeTest exception: " + ex.toString());
		}
	}

	public static String setDeveloperId(String developerId)
	{
		try
		{
			Log.i("Unity", "setDeveloperId developerId: " + developerId);
			m_developerId = developerId;
		}
		catch (Exception ex) 
		{
			Log.i("Unity", "setDeveloperId exception: " + ex.toString());
		}
		return "";
	}

	public static void getProductsAsync()
	{
		try
		{
			Log.i("Unity", "OuyaUnityPlugin.getProductsAsync");

			if (m_enableIAP)
			{
				Log.i("Unity", "OuyaUnityPlugin.getProductsAsync IAP is enabled");
				InitializeTest();
			}
			else
			{
				Log.i("Unity", "OuyaUnityPlugin.getProductsAsync IAP is disabled");
				return;
			}

			if (null == m_test)
			{
				Log.i("Unity", "OuyaUnityPlugin: m_test is null");
			}
			else
			{
				Log.i("Unity", "OuyaUnityPlugin: m_test is valid");
				m_test.requestProducts();
			}
		}
		catch (Exception ex) 
		{
			Log.i("Unity", "OuyaUnityPlugin: getProductsAsync exception: " + ex.toString());
		}
	}

	public static void clearGetProductList()
	{
		try
		{
			Log.i("Unity", "clearGetProductList");
		
			TestOuyaFacade.PRODUCT_IDENTIFIER_LIST.clear();
		}
		catch (Exception ex) 
		{
			Log.i("Unity", "clearGetProductList exception: " + ex.toString());
		}
	}

	public static void addGetProduct(String productId)
	{
		try
		{
			Log.i("Unity", "addGetProduct productId: " + productId);
		
			boolean found = false;
			for (Purchasable purchasable : TestOuyaFacade.PRODUCT_IDENTIFIER_LIST)
			{
				//Log.i("Unity", "addGetProduct " + purchasable.getProductId() + "==" + productId);
				if (purchasable.getProductId().equals(productId))
				{
					//Log.i("Unity", "addGetProduct equals: " + purchasable.getProductId() + "==" + productId + "=" + purchasable.getProductId().equals(productId));
					found = true;
					break;
				}
			}
			if (found)
			{
				//Log.i("Unity", "addGetProduct found productId: " + productId);
			}
			else
			{
				//Log.i("Unity", "addGetProduct added productId: " + productId);
				Purchasable newPurchasable = new Purchasable(new String(productId));
				TestOuyaFacade.PRODUCT_IDENTIFIER_LIST.add(newPurchasable);
			}
		}
		catch (Exception ex) 
		{
			Log.i("Unity", "addGetProduct exception: " + ex.toString());
		}
	}

	public static void debugGetProductList()
	{
		try
		{
			int count = 0;
			for (Purchasable purchasable : TestOuyaFacade.PRODUCT_IDENTIFIER_LIST)
			{
				++count;
			}
			Log.i("Unity", "debugProductList TestOuyaFacade.PRODUCT_IDENTIFIER_LIST has " + count + " elements");
			for (Purchasable purchasable : TestOuyaFacade.PRODUCT_IDENTIFIER_LIST)
			{
				Log.i("Unity", "debugProductList TestOuyaFacade.PRODUCT_IDENTIFIER_LIST has: " + purchasable.getProductId());
			}
		}
		catch (Exception ex) 
		{
			Log.i("Unity", "debugProductList exception: " + ex.toString());
		}
	}

	public static String requestPurchaseAsync(String sku)
	{
		try
		{
			Log.i("Unity", "requestPurchaseAsync sku: " + sku);
		
			if (m_enableIAP)
			{
				InitializeTest();
			}
			else
			{
				return "";
			}

			if (null == m_test)
			{
				Log.i("Unity", "m_test is null");
			}
			else
			{
				Log.i("Unity", "m_test is valid");
				Product product = new Product();
				product.setIdentifier(sku);
				m_test.requestPurchase(product);
			}
		}
		catch (Exception ex) 
		{
			Log.i("Unity", "requestPurchaseAsync exception: " + ex.toString());
		}
		return "";
	}

	public static void getReceiptsAsync()
	{
		try
		{
			Log.i("Unity", "OuyaUnityPlugin.getReceiptsAsync");

			if (m_enableIAP)
			{
				Log.i("Unity", "OuyaUnityPlugin.getReceiptsAsync IAP is enabled");
				InitializeTest();
			}
			else
			{
				Log.i("Unity", "OuyaUnityPlugin.getReceiptsAsync IAP is disabled");
				return;
			}

			if (null == m_test)
			{
				Log.i("Unity", "OuyaUnityPlugin: m_test is null");
			}
			else
			{
				Log.i("Unity", "OuyaUnityPlugin: m_test is valid");
				m_test.requestReceipts();
			}
		}
		catch (Exception ex) 
		{
			Log.i("Unity", "OuyaUnityPlugin: getProductsAsync exception: " + ex.toString());
		}
	}
}
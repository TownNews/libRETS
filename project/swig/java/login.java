import java.lang.*;
import librets.*;

public class login 
{
    static
    {
        try
        {
            System.loadLibrary("librets");
        }
        catch (UnsatisfiedLinkError e)
        {
            System.err.println("Unable to load the librets native library.\n" + e);
            System.exit(1);
        }
    }
    

    public static void main(String argv[]) throws Exception
    {
        RetsSession session = new RetsSession("http://demo.crt.realtors.org:6103/rets/login");

	try {
        if (!session.Login("Joe", "Schmoe"))
        {
            System.out.println("Invalid login");
            System.exit(2);
        }

        LoginResponse response = session.GetLoginResponse();
        CapabilityUrls capurls = session.GetCapabilityUrls();

        System.out.println("Member name: " + response.GetMemberName());
        System.out.println("Search URL: " + capurls.GetSearchUrl());

        String version = "1.0";
        if (session.GetDetectedRetsVersion() == RetsVersion.RETS_1_5)
        {
            version = "1.5";
        }

        System.out.println("RETS Version: " + version);

        LogoutResponse logout = session.Logout();

        System.out.println("Billing info: " + logout.GetBillingInfo());
        System.out.println("Logout Message:  " + logout.GetLogoutMessage());
        System.out.println("Connect time: " + logout.GetConnectTime());
	} 
	catch (RetsHttpException ex) 
	{
	    System.out.println(ex.getMessage());
	}
    }
}

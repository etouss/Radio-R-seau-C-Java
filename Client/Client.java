import java.util.HashMap;
import java.util.Scanner;
public class Client 
{
	// java -Djava.net.preferIPv4Stack=true Client
	// Tous les elements necessaires pour le client
	static Scanner sc = null;
	static MenuDiffuseur menu_diffuseur = null;
	static HashMap<String, EcouterDiffuseur> tableau_diffuseurs_choisis = new HashMap<String, EcouterDiffuseur>();
	static String id_client = "Default";
	static int port_gestionnaire = 0;
	static String addr_gestionnaire = null;
	static boolean verbose = false;
	
	// Change l'identifiant de l'utilisateur
	public static boolean changeLogin()
	{
		// 'flush' du scanner
		sc.nextLine();
		String name = sc.nextLine();
		
		// Si l'utilisateur n'a rien rentre, c'est qu'il ne veut pas changer son identifiant
		if(name.length() == 0)
			return false;
		
		// Verification du login rentre
		while(name.length() > 8)
		{
			System.out.println("Le login doit etre d'au plus 8 caracteres, recommencez :");
			name = sc.nextLine();
		}
		
		// L'identifiant est valide, on change
		id_client = name;
		return true;
	}

	// Fonction principale
	public static void main(String[] args) 
	{
		if(args.length == 1 && args[0].equals("-v"))
			verbose = true;
		System.setProperty("java.net.preferIPv4Stack","true");
		sc = new Scanner(System.in);
		if(verbose)
			System.out.println("/////------------ Menu principal lance ------------/////");
		new MenuPrincipal().lancerMenu();
		sc.close();
	}
}

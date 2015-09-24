import java.util.InputMismatchException;

public class MenuGestionnaire implements Menu
{
	public void lancerMenu()
	{
		String[] tab_diffuseurs = new ConnectGest().connexion_gestionnaire();

		// Si on a eu un problème lors de la demande au gestionnaire des diffuseurs disponibles
		if(tab_diffuseurs == null)
		{
			if(Client.verbose)
				System.out.println("/////------------ Tableau des diffuseurs null ------------/////");
			new MenuPrincipal().lancerMenu();
			return;
		}

		// L'utilisateur choisit sa radio;
		System.out.println("\n -----------------------------------------------");
		System.out.println("| Quelle(s) radio(s) voulez vous ecouter ?");
		System.out.print("| Vous pouvez en choisir plusieurs en mettant plusieurs choix à la suite separes par des espaces : ");

		// Recuperation des choix de l'utilisateur
		Client.sc.nextLine();
		String numeros = Client.sc.nextLine();
		int [] tableau_choix = choix_tableau(numeros, tab_diffuseurs.length);
		if(tableau_choix == null)
		{
			if(Client.verbose)
				System.out.println("/////------------ Pas de choix, relancement du menu principal ------------/////");
			new MenuPrincipal().lancerMenu();
		}
		else
		{
			if(Client.verbose)
				System.out.println("/////------------ Choix conformes, lancement du/des diffuseur(s)... ------------/////");
			Client.menu_diffuseur = new MenuDiffuseur(tab_diffuseurs,tableau_choix);
			Client.menu_diffuseur.lancerMenu();
		}
	}

	public int[] choix_tableau(String message, int taille)
	{
		if(Client.verbose)
			System.out.println("/////------------ Verification des choix donnes... ------------/////");
		String[] splits = message.split(" ");
		int[] result = new int[splits.length];	// Tableau final ayant les numeros des radios a ecouter
		while(true)
		{
			boolean sestTrompe = false;	// Correction bug Scanner
			
			// On parcours le tableau des choix donnes par l'utilisateur
			for(int i=0; i<splits.length; i++)
			{
				try
				{
					// Si l'utilisateur n'a pas donne des entier, on relance le programme
					int nb = Integer.parseInt(splits[i]);
					if(nb == 0)
						return null;
					
					// Si le choix n'est pas bon, on redemande un nouvel entier
					while(nb<1 || nb>taille)
					{
						System.out.println("\nLe choix numero " + (i+1) + " n'est pas bon, il faut des nombres entre 1 et " + taille + " : ");
						nb = Client.sc.nextInt();
						sestTrompe = true;
					}

					// MAJ du tableau final des choix
					result[i] = nb;
				}
				catch(InputMismatchException e)
				{
					System.out.println("\nVous devez rentrer des numeros de radios, recommencez :");
					Client.sc.nextLine();
					continue;
				}
				catch (Exception e)
				{
					System.out.println("Vous deviez rentrer des numeros de radios, voici la liste");
					new MenuGestionnaire().lancerMenu();
					return null;
				}
			}

			// Si on a du re entrer des choses en ligne de commande, obligation d'avancer le Scanner sinon bug
			if(sestTrompe)
				Client.sc.nextLine();

			return result;
		}
	}
}
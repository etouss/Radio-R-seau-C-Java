import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.net.Socket;

public class ConnectGest
{
	// Ecrit sur la sortie standart ces diffuseurs, pour renvoyer le tableau remplie de ceux-ci
	public String[] liste_diffuseurs(int nb_diffuseurs, BufferedReader br)
	{
		if(Client.verbose)
			System.out.println("/////------------ Demande d'affichage de la liste des diffuseurs... ------------/////");
		if(nb_diffuseurs == 0)
		{
			System.out.println("Aucun diffuseur n'est disponible...");
			return null;
		}
		
		int i=1;
		System.out.println("\n		 --------------------------------------");
		System.out.println("		| Tableau des diffuseurs disponibles : |");
		System.out.println("		|                                      |");
		String[] tab_diffuseurs = new String[nb_diffuseurs];
		while(nb_diffuseurs != 0)
		{
			String msg = null;
			try
			{
				msg = br.readLine();
				String[] splits = msg.split(" ");
				if (splits[0].equals("ITEM")) 
				{
					tab_diffuseurs[i-1] = msg;
					System.out.println("\t	|	 " + i + " -> " + MenuDiffuseur.nom_propre(splits[1]) + "                 |");
				}
				else
				{
					System.out.println("Erreur : le gestionnaire a eu un probleme (liste diffuseurs).");
					return null;
				}
				nb_diffuseurs--;
				i++;
			}
			catch (Exception e)
			{
        e.printStackTrace();
				System.out.println("Erreur : le gestionnaire ne repond pas");
				return null;
			}
		}
		System.out.println("\t	|	 0 -> Annuler                  |");
		System.out.println("		 --------------------------------------");
		if(Client.verbose)
			System.out.println("/////------------ Tableau affiché, envoi de celui-ci ------------/////");
		return tab_diffuseurs;
	}

	// Se connecte au gestionnaire pour lui demander les diffuseurs disponibles
	public String[] connexion_gestionnaire()
	{
		try
		{
			if(Client.verbose)
				System.out.println("/////------------ Lancement de la connexion au gestionnaire... ------------/////");
			int nb_diffuseurs = 0;
			Socket sock = new Socket(Client.addr_gestionnaire, Client.port_gestionnaire);
			BufferedReader br = new BufferedReader(new InputStreamReader(sock.getInputStream()));
			PrintWriter pw = new PrintWriter(new OutputStreamWriter(sock.getOutputStream()));
			
			if(Client.verbose)
				System.out.println("/////------------ Connexion au gestionnaire reussie ------------/////");
			String [] splits;
			try
			{
				if(Client.verbose)
					System.out.println("/////------------ Envoi message LIST au gestionnaire ------------/////");
				pw.print("LIST\r\n");
				pw.flush();

				if(Client.verbose)
					System.out.println("/////------------ Reception reponse du gestionnaire ------------/////");
				splits = br.readLine().split(" ");
				if (splits[0].equals("LINB"))
				{
					if(Client.verbose)
						System.out.println("/////------------ Message LINB conforme reçu ------------/////");
					try
					{
						nb_diffuseurs = Integer.parseInt(splits[1]);
						if(nb_diffuseurs < 0 || nb_diffuseurs > 999)
						{
							System.out.println("Erreur : le gestionnaire a envoye un mauvais nombre de diffuseurs.");
							sock.close();
							return null;
						}
					}
					catch (NumberFormatException e) 
					{
						System.out.println("Erreur : le gestionnaire aurait du renvoyer un nombre de diffuseur");
						br.close();
						pw.close();
						sock.close();
						return null;
					}
				}
				else
				{
					if(Client.verbose)
						System.out.println("/////------------ Mauvais message gestionnaire reçu  ------------/////");
					System.out.println("Erreur : le gestionnaire n'a pas envoye le message LINB pour le nombre de diffuseurs.");
					br.close();
					pw.close();
					sock.close();
					return null;
				}
			}
			catch (Exception e) 
			{
				System.out.println("Erreur : le gestionnaire a eu un problème (rien envoye).");
				sock.close();
				return null;
			}

			if(Client.verbose)
				System.out.println("/////------------ Fermeture connexion avec le gestionnaire ------------/////");
			// le gestionnaire a donné la liste des diffuseurs, on ferme la connexion
			String[] tab_tmp = liste_diffuseurs(nb_diffuseurs,br);
			sock.close();
			return tab_tmp;
		} 
		catch (Exception e) 
		{
			System.out.println("Connexion socket n'a pas marchee.");
		}
		return null;
	}
}

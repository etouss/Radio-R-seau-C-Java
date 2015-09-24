import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.net.Socket;
import java.util.ArrayList;
import java.util.HashMap;

public class MenuDiffuseur implements Menu {
	private int[] tab_index_diffuseurs;	// Tableau contenant les numeros des diffuseurs choisis au debut du programme
	private String[] tableau;	// Tableau contenant les diffuseurs disponibles a l'ecoute
	private HashMap<Integer,String> tab_menus;
	private boolean ecrireHistoire = false;
	private boolean downloadHistoire = false;
	private boolean play = false;

	// Constructeur et mis a jour des tableaux de diffuseurs et de choix
	public MenuDiffuseur(String[] _tableau, int[] _tab_diffuseurs)
	{
		if(Client.verbose)
			System.out.println("/////------------ Initialisation des diffuseurs ------------/////");
		tab_index_diffuseurs = _tab_diffuseurs;
		tableau = _tableau;
		tab_menus = new HashMap<Integer,String>();
		tab_menus.put(0, "Revenir au menu principal");
		tab_menus.put(1, "Envoyer un message à une radio");
		tab_menus.put(2, "Demander un certain nombre de messages");
		tab_menus.put(3, "Arreter d'écouter une radio");
		tab_menus.put(4, "Ecouter une nouvelle radio");
		tab_menus.put(5, "Demander a la radio les informations sur celle-ci");
		tab_menus.put(6, "Aider à construire l'histoire");
		tab_menus.put(7, "Télécharger l'histoire écrite par la communautée");
		tab_menus.put(8, "Jouer au jeu");
		tab_menus.put(10, "Activer mode verbose");
		tab_menus.put(11, "Desactiver mode verbose");
	}

	public void affichageMenu()
	{
		for(int key: tab_menus.keySet())
		{
			if( 
					(!Client.verbose && key==10) ||
					(Client.verbose && key==11) ||
					(Client.tableau_diffuseurs_choisis.size()>0 && (key==0 || key==1 || key==2 || key==3 || key==4 || key==5)) ||
					(Client.tableau_diffuseurs_choisis.size()<=0 && (key==0 || key==4)) ||
					(Client.tableau_diffuseurs_choisis.size()>0 && ecrireHistoire && key==6) || 
					(Client.tableau_diffuseurs_choisis.size()>0 && downloadHistoire && key==7) ||
					(Client.tableau_diffuseurs_choisis.size()>0 && play && key==8)
			)
				System.out.println("		|	"+key+" -> "+tab_menus.get(key));
		}
	}
	
	public void lancerMenu()
	{
		if(Client.verbose)
			System.out.println("/////------------ Recuperation des choix ------------/////");
		// On lance chaque radio dans un terminal different
		
		System.out.println("\n  -----------------------------------------------");
		for(int index: tab_index_diffuseurs)
		{
			// Recuperation attributs diffuseur
			String[] diffuseur = tableau[index-1].split(" ");
			String nom_diffuseur = nom_propre(diffuseur[1]);
			if(Client.tableau_diffuseurs_choisis.containsKey(nom_diffuseur))
			{
				System.out.println("!!!! Cette radio est deja en ecoute");
				continue;
			}
			String addr_diffuseurTCP = diffuseur[4];
			String addr_diffuseurUDP = diffuseur[2];
			int port_diffuseurUDP = Integer.parseInt(diffuseur[3]);
			int port_diffuseurTCP = Integer.parseInt(diffuseur[5]);

			// On demande a l'utilisateur dans quel terminal il veut que sa radio soit diffusee
			System.out.print("| Veuillez donner le nom du terminal ou vous voulez ecouter la radio " + nom_diffuseur + " :  ");
			String nom_terminal = Client.sc.nextLine();
			
			// On lance dans l'autre terminal l'ecoute de la radio choisie
			Client.tableau_diffuseurs_choisis.put(nom_diffuseur, new EcouterDiffuseur(nom_terminal,nom_diffuseur,addr_diffuseurTCP,addr_diffuseurUDP,port_diffuseurUDP,port_diffuseurTCP));
		}
		if(Client.verbose && tab_index_diffuseurs.length!=0)
			System.out.println("/////------------ Diffuseurs lances, debut des threads... ------------/////");

		// On lance tous les Thread de leur cote
		for(String key : Client.tableau_diffuseurs_choisis.keySet())
			ajouterThread(key);

		if(Client.verbose && tab_index_diffuseurs.length!=0)
			System.out.println("/////------------ Tous les threads ont debutes, lancement menu... ------------/////");
		menu();
	}

	// Methode servant au message LAST qui envoie et recupere des messages au diffuseur
	public void recupererMessageDiffuseur(int nb, String addr_diffuseurTCP, int port_diffuseurTCP)
	{
		if(Client.verbose)
			System.out.println("/////------------ Demande de recuperation de messages ------------/////");
		try
		{
			if(Client.verbose)
				System.out.println("/////------------ Initialisation socket et connections... ------------/////");
			Socket sock = new Socket(addr_diffuseurTCP, port_diffuseurTCP);	
			PrintWriter pw = new PrintWriter(new OutputStreamWriter(sock.getOutputStream()));
			if(Client.verbose)
				System.out.println("/////------------ Connexion faite avec le diffuseur en TCP ------------/////");

			// Demande au diffuseur des nb derniers messages
			pw.write("LAST " + nb + "\r\n");
			pw.flush();
			if(Client.verbose)
				System.out.println("/////------------ Envoie du message LAST fait ------------/////");

			System.out.println("\n\nDemande de " + nb + " messages :");
			int nombre_recu = 0;
			if(Client.verbose)
				System.out.println("/////------------ Recuperation des données envoyees par diffuseur ------------/////");
			// On lit au plus nb fois ce que le diffuseur nous envoie
			while(true)
			{
				if(Client.verbose)
					System.out.println("/////------------ Message reçu ------------/////");
				byte[] data = new byte[161];
				sock.getInputStream().read(data,0,161);
				String mess = new String(data);

				// Message stipulant que le diffuseur a fini d'envoyer
				if(mess.startsWith("ENDM"))
				{
					System.out.println("\n ---------------------------------------------------");
					if(nombre_recu < nb)
						System.out.println("Le diffuseur vous a envoyé moins de message que le nombre demandé.");
					else
						System.out.println("Le diffuseur vous a envoyé tous les messages demandés.\n");
					break;
				}

				String[] message = mess.split(" ",4);
				System.out.println("\n ---------------------------------------------------");
				System.out.println("Message numéro " + message[1] + " : " + nom_propre(message[3]));
				nombre_recu++;
			}
			if(Client.verbose)
				System.out.println("/////------------ Fermeture connexions...  ------------/////");
			pw.close();
			sock.close();
			if(Client.verbose)
				System.out.println("/////------------ Fin de demande de message ------------/////");
		}
		catch (Exception e) 
		{
			e.printStackTrace();
			System.out.println("Nous ne pouvons pas vous connecter sur le diffuseur pour le moment. (LAST)");
		}
	}

	public String envoyer_nom(String nom)
	{
		if(Client.verbose)
			System.out.println("/////------------ Preparation pour envoi... ------------/////");
		
		while(nom.length() < 8)
			nom+= '#';
		
		if(Client.verbose)
			System.out.println("/////------------ Nom proprement prepare pour envoi ------------/////");
		return nom;
	}

	// Methode servant au MESS de l'utilisateur qui envoit un messgage devant etre diffusé sur le diffuseur
	public void envoyerMessageDiffuseur(String mess, String addr_diffuseurTCP, int port_diffuseurTCP)
	{
		if(Client.verbose)
			System.out.println("/////------------ Preparation envoi message au diffuseur ------------/////");
		try
		{
			if(Client.verbose)
				System.out.println("/////------------ Initialisation des connexions... ------------/////");
			Socket sock = new Socket(addr_diffuseurTCP, port_diffuseurTCP);
			BufferedReader br = new BufferedReader(new InputStreamReader(sock.getInputStream()));
			PrintWriter pw = new PrintWriter(new OutputStreamWriter(sock.getOutputStream()));
			if(Client.verbose)
				System.out.println("/////------------ Connexion faites, envoi du message MESS ------------/////");
			pw.write("MESS " + envoyer_nom(Client.id_client) + " " + preparerMessageTCP(mess) + "\r\n");
			pw.flush();
			
			if(Client.verbose)
				System.out.println("/////------------ Reception reponse du diffuseur (ACKM)... ------------/////");
			// Message de retour du diffuseur
			if(!br.readLine().equals("ACKM"))
				System.out.println("Un probleme s'est produit avec le diffuseur");

			if(Client.verbose)
				System.out.println("/////------------ Fermeture connexions ------------/////");
			// Fermeture des connexion
			pw.close();
			br.close();
			sock.close();
		}
		catch (Exception e)
		{
			System.out.println("Nous ne pouvons pas vous connecter sur le diffuseur pour le moment. (MESS)");
		}
	}

	public void ajouterThread(String key)
	{
		Thread t = new Thread(Client.tableau_diffuseurs_choisis.get(key));
		Client.tableau_diffuseurs_choisis.get(key).setThread_courant(t);
		t.start();
		if(Client.verbose)
			System.out.println("/////------------ Thread ajoute et lance ------------/////");
	}

	// Si un message ne fait pas 140 caracteres, on le remplit avec des #
	public String preparerMessageTCP(String mess)
	{
		if(Client.verbose)
			System.out.println("/////------------ Preparation message pour envoi... ------------/////");
		
		while(mess.length()<140)
			mess += '#';
		if(Client.verbose)
			System.out.println("/////------------ Message pret pour envoi ------------/////");
		
		return mess;
	}

	// Renvoyant le nom en remplacant les # par des espaces
	static public String nom_menu(String nom)
	{
		if(Client.verbose)
			System.out.println("/////------------ Preparation nom menu... ------------/////");
		String result = "";
		for(int i=0; i<nom.length(); i++)
		{
			// Si deux # sont à la suite, on renvoie le nom directement
			if(nom.toCharArray()[i]=='#')
				result += ' ';
			else
				result += nom.toCharArray()[i];
		}
		
		if(Client.verbose)
			System.out.println("/////------------ Nom menu pret ------------/////");
		return result;
	}


	// Renvoyant le nom sans #
	static public String nom_propre(String nom)
	{
		String result = "";
		for(int i=0; i<nom.length(); i++)
		{
			// Si deux # sont à la suite, on renvoie le nom directement
			if(nom.toCharArray()[i]=='#' && i<nom.length()-1 && nom.toCharArray()[i+1]=='#')
				return result;
			else
				result += nom.toCharArray()[i];
		}
		return nom;
	}

	// Affiche chaque nom des diffuseurs disponibles et retourne le choix
	public static String quelDiffuseur()
	{
		if(Client.verbose)
			System.out.println("/////------------ Lancement de l'affichage de la liste diffuseurs pour choix... ------------/////");
		while(true)
		{
			int i=1;
			int taille = Client.tableau_diffuseurs_choisis.size();
			String[] tab = new String[taille];

			for(String nom: Client.tableau_diffuseurs_choisis.keySet())
			{
				tab[i-1] = nom;
				System.out.println("|\t" + i + " -> " + nom);
				i++;
			}
			int index = -1;
			try
			{
				System.out.print("Votre choix : ");
				Client.sc.nextLine();
				index = Client.sc.nextInt();
				while(index < 1 || index > taille)
				{
					System.out.print("Vous pouvez choisir entre 1 et " + taille + " : ");
					index = Client.sc.nextInt();
				}
				
				if(Client.verbose)
					System.out.println("/////------------ Choix du diffuseur réussi ------------/////");
				return tab[index-1];
			}
			catch (Exception e)
			{
				System.out.println("Vous devez rentrer un numero, voici la liste avec les choix disponibles");
			}
		}
	}

	public void menu()
	{
		if(Client.verbose)
			System.out.println("/////------------ Lancement menu ------------/////");
		boolean exit = false;
		while(true)
		{
			System.out.print("\n		 Login : " + Client.id_client);
			System.out.println("\n		 --------------------------------------------------------------");
			System.out.println("		| Que voulez vous faire ?                           ");
			System.out.println("		|                                                   ");
			affichageMenu();
			System.out.println("		 --------------------------------------------------------------");
			System.out.print("Votre choix : ");
			// Parallelement on regarde ce que l'utilisateur veut faire autre que de lire la radio
			int choix = 0;
			try
			{
				choix = Client.sc.nextInt();
			}
			catch (Exception e)
			{
				System.out.println("Il faut rentrer un numero, voici la liste disponible");
				Client.sc.nextLine();
				continue;
			}
			if(Client.verbose)
				System.out.println("/////------------ Choix (int) conforme ------------/////");

			// Nombre de choix different en fonction de s'il y a des diffuseurs actifs ou non
			if(Client.tableau_diffuseurs_choisis.size()>0)
			{
				while(choix < 0 || choix > 11)
				{
					System.out.println("Mauvais choix, recommencez: ");
					choix = Client.sc.nextInt();
				}
			}
			// On ne peut que ajouter des diffuseurs ou quitter
			else
			{
				while(choix != 0 && choix != 4)
				{
					System.out.println("Seulement 2 choix sont disponibles, veuillez recommencer:");
					choix = Client.sc.nextInt();
				}
			}

			if(Client.verbose)
				System.out.println("/////------------ Choix conforme, lancement de la methode correspondante... ------------/////");
			// Gestion du choix de l'utilisateur
			switch(choix)
			{
			case 1:
			{
				if(Client.verbose)
					System.out.println("/////------------ Envoi message demande ------------/////");
				// On recupere les informations du diffuseur a qui on veut envoyer un message
				System.out.println("\n ---------------------------------------------------");
				System.out.println("| A quelle radio voulez vous envoyer un message ?");
				String nom = quelDiffuseur();

				// On recupere maintenant le message a envoyer
				System.out.println("\n ---------------------------------------------------");
				System.out.print("| Ecrivez simplement le message que vous voulez diffuser : ");
				Client.sc.nextLine();
				String mess = Client.sc.nextLine();
				while(mess.length() > 140)
				{
					System.out.println("Vous devez ecrire un message de seulement 140 caractères, recommencez:");
					mess = Client.sc.nextLine();
				}
				// On envoit le message en TCP 
				envoyerMessageDiffuseur(mess,Client.tableau_diffuseurs_choisis.get(nom).getAddrTCP(),Client.tableau_diffuseurs_choisis.get(nom).getPortTCP());
				break;
			}
			case 2:
			{
				if(Client.verbose)
					System.out.println("/////------------ Demande de message fait ------------/////");
				while(true)
				{
					// On recupere les informations du diffuseur a qui on veut recuperer les messages
					System.out.println("\n ---------------------------------------------------");
					System.out.println("| A quelle radio voulez vous demander les messages ?");
					String nom = quelDiffuseur();

					System.out.println("\n ---------------------------------------------------");
					System.out.println("| Ecrivez simplement le nombre de messages que vous voulez revoir:");
					int nb = -1;
					if(Client.verbose)
						System.out.println("/////------------ Client choisit nombre de messages voulus... ------------/////");
					try
					{
						nb = Client.sc.nextInt();
						while(nb > 999 || nb < 0)
						{
							System.out.println("!!! Vous devez ecrire un nombre comprit entre 0 et 999, recommencez:");
							nb = Client.sc.nextInt();
						}
					}
					catch (Exception e) 
					{
						System.out.println("!!! Veuillez rentrer un nombre");
						continue;
					}
					if(Client.verbose)
						System.out.println("/////------------ Choix fait ------------/////");
					// On recupere les messages en TCP
					recupererMessageDiffuseur(nb, Client.tableau_diffuseurs_choisis.get(nom).getAddrTCP(), Client.tableau_diffuseurs_choisis.get(nom).getPortTCP());
					break;
				}
				break;
			}
			case 3:
			{
				suppressionRadio();
				break;
			}
			case 4:
			{
				if(Client.verbose)
					System.out.println("/////------------ Nouvelle radio a ecouter ------------/////");
				int index = -1;
				boolean quitter = false;
				while(true)
				{
					int i=1;
					System.out.println("\n ------------------------------------");
					System.out.println("| Quelle nouvelle radio voulez-vous ecouter ?");
					for(String diffus :tableau)
					{
						System.out.println("|	" + i + " -> " + nom_menu(diffus.split(" ")[1]));
						i++;
					}
					System.out.println("|\t0 -> Annuler");
					System.out.print("Votre choix : ");
					Client.sc.nextLine();

					try
					{
						index = Client.sc.nextInt();
						if(index == 0)
						{
							quitter = true;
							break;
						}

						while(index<0 || index>i || Client.tableau_diffuseurs_choisis.containsKey(nom_propre(tableau[index-1].split(" ")[1])))
						{
							System.out.println("!!! Le nombre n'est pas bon ou la radio est deja ecoutee, recommencez :");
							index = Client.sc.nextInt();
						}
					}
					catch (Exception e) 
					{
						System.out.println("!!! Veuillez rentrer un nombre");
						continue;
					}
					break;
				}

				if(quitter)
					break;

				if(Client.verbose)
					System.out.println("/////------------ Recuperation des attributs du diffuseur... ------------/////");
				
				// Recuperation attributs diffuseur
				String[] diffuseur = tableau[index-1].split(" ");
				String nom_diffuseur = nom_propre(diffuseur[1]);
				String addr_diffuseurTCP = diffuseur[4];
				String addr_diffuseurUDP = diffuseur[2];
				int port_diffuseurUDP = Integer.parseInt(diffuseur[3]);
				int port_diffuseurTCP = Integer.parseInt(diffuseur[5]);

				// On demande a l'utilisateur dans quel terminal il veut que sa radio soit diffusee
				System.out.println("\n ------------------------------------");
				System.out.println("| Veuillez donner le nom du terminal ou vous voulez ecouter cette nouvelle radio " + nom_diffuseur + " :");
				Client.sc.nextLine();
				String nom_terminal = Client.sc.nextLine();

				if(Client.verbose)
					System.out.println("/////------------ Lancement de la radio dans un nouveau terminal... ------------/////");
				
				// On lance dans l'autre terminal l'ecoute de la radio choisie
				Client.tableau_diffuseurs_choisis.put(nom_diffuseur, new EcouterDiffuseur(nom_terminal,nom_diffuseur,addr_diffuseurTCP,addr_diffuseurUDP,port_diffuseurUDP,port_diffuseurTCP));
				ajouterThread(nom_diffuseur);
				if(Client.verbose)
					System.out.println("/////------------ Radio lancee dans le terminal "+nom_terminal+" ------------/////");
				
				break;
			}
			case 0:
				if(Client.verbose)
					System.out.println("/////------------ Fermeture menu ------------/////");
				exit = true; break;
			case 5:
			{
				if(Client.verbose)
					System.out.println("/////------------ Demande informations... ------------/////");
				System.out.println("\n ---------------------------------------------------");
				System.out.println("| De quelle radio voulez-vous les informations ?");
				String nom = quelDiffuseur();
				
				// On envoit le message en TCP 
				demanderInfosDiffuseur(Client.tableau_diffuseurs_choisis.get(nom).getAddrTCP(),Client.tableau_diffuseurs_choisis.get(nom).getPortTCP());
				if(Client.verbose)
					System.out.println("/////------------ Informations transmises ------------/////");
			} break;
			case 6:
			{
				System.out.println("\n ---------------------------------------------------");
				System.out.println("| A quelle radio voulez-vous partager votre histoire ?");
				String nom = quelDiffuseur();
				
				// On envoit le message en TCP 
				envoiHistoire(Client.tableau_diffuseurs_choisis.get(nom).getAddrTCP(),Client.tableau_diffuseurs_choisis.get(nom).getPortTCP());
			} break;
			
			case 7:	
			{
				System.out.println("\n ---------------------------------------------------");
				System.out.println("| De quelle radio voulez vous recuperer l'histoire ?");
				String nom = quelDiffuseur();
				
				// On envoit le message en TCP 
				affichageHistoire(Client.tableau_diffuseurs_choisis.get(nom).getAddrTCP(),Client.tableau_diffuseurs_choisis.get(nom).getPortTCP());
			} break;
			case 8:
			{
				System.out.println("\n ---------------------------------------------------");
				System.out.println("| Avec quelle radio voulez vous jouer ?");
				String nom = quelDiffuseur();
				
				// On envoit le message en TCP 
				playGame(Client.tableau_diffuseurs_choisis.get(nom).getAddrTCP(),Client.tableau_diffuseurs_choisis.get(nom).getPortTCP());
			} break;
			
			case 10: Client.verbose = true; break;
			case 11: Client.verbose = false; break;
			// Fin du programme
			default: System.out.println("!!! Probleme menu diffuseur."); continue;
			}

			if(exit)
				break;
		}
	}
	
	public void affichageHistoire(String addr_diffuseurTCP, int port_diffuseurTCP)
	{
		if(Client.verbose)
			System.out.println("/////------------ Demande d'affichage de l'histoire ------------/////");
		try
		{
			Socket sock = new Socket(addr_diffuseurTCP, port_diffuseurTCP);	
			PrintWriter pw_ = new PrintWriter(new OutputStreamWriter(sock.getOutputStream()));
			pw_.write("DOWN\r\n");
			pw_.flush();
			byte[] data_ = new byte[161];
			sock.getInputStream().read(data_,0,161);
			String mess = new String(data_);
			System.out.println(nom_propre(mess).substring(5));
			
			sock.close();
		}
		catch (Exception e)
		{
			System.out.println("Nous ne pouvons pas vous connecter sur le diffuseur pour le moment. (MESS)");
		}
		if(Client.verbose)
			System.out.println("/////------------ Affichage de l'histoire fait ------------/////");
	}
	
	public void playGame(String addr_diffuseurTCP, int port_diffuseurTCP)
	{
		if(Client.verbose)
			System.out.println("/////------------ Envoi valeur pour le jeu ------------/////");
		try
		{
			Socket sock = new Socket(addr_diffuseurTCP, port_diffuseurTCP);	
			PrintWriter pw_ = new PrintWriter(new OutputStreamWriter(sock.getOutputStream()));
			System.out.print("Votre numéro : ");
			int choix = 0;
			
			while(true)
			{
				try
				{
					choix = Client.sc.nextInt();
				}
				catch (Exception e)
				{
					System.out.println("Il faut rentrer un numero...");
					Client.sc.nextLine();
					continue;
				}
				break;
			}
			pw_.write("PLAY "+choix+"\r\n");
			pw_.flush();
			sock.close();
		}
		catch (Exception e)
		{
			System.out.println("Nous ne pouvons pas vous connecter sur le diffuseur pour le moment. (playGame)");
		}
		if(Client.verbose)
			System.out.println("/////------------ Coup envoyé pour le jeu ------------/////");
	}

	
	public void envoiHistoire(String addr_diffuseurTCP, int port_diffuseurTCP)
	{
		if(Client.verbose)
			System.out.println("/////------------ Envoi histoire... ------------/////");
		try
		{
			Socket sock = new Socket(addr_diffuseurTCP, port_diffuseurTCP);	
			PrintWriter pw_ = new PrintWriter(new OutputStreamWriter(sock.getOutputStream()));
			System.out.println("Le texte à ajouter à l'histoire :");
			Client.sc.nextLine();
			String histoire = Client.sc.nextLine();
			while(histoire.length() > 140)
			{
				System.out.println("Votre histoire dépasse les 140 caractères!");
				histoire = Client.sc.nextLine();
			}
			pw_.write("VOTE "+histoire+"\r\n");
			pw_.flush();
			
			byte[] data_ = new byte[161];
			sock.getInputStream().read(data_,0,161);
			String mess = new String(data_);
			if(mess.startsWith("ER"))
				System.out.println("Ce diffuseur ne marche pas avec le mode histoire...");
			else
				System.out.println("Histoire envoyée");
			sock.close();
		}
		catch (Exception e)
		{
			System.out.println("Nous ne pouvons pas vous connecter sur le diffuseur pour le moment. (envoiHistoire)");
		}
		if(Client.verbose)
			System.out.println("/////------------ Histoire envoyée ------------/////");
	}
	
	public void demanderInfosDiffuseur(String addr_diffuseurTCP, int port_diffuseurTCP)
	{
		if(Client.verbose)
			System.out.println("/////------------ Demande Infos Diffuseurs... ------------/////");
		ArrayList<String> instr = new ArrayList<String>();
		byte[] data_ = new byte[148];
		byte[] data = new byte[6];
		try
		{
			Socket sock = new Socket(addr_diffuseurTCP, port_diffuseurTCP);	
			PrintWriter pw_ = new PrintWriter(new OutputStreamWriter(sock.getOutputStream()));
			pw_.write("INFO\r\n");
			pw_.flush();
			sock.getInputStream().read(data_,0,148);
			String mess = new String(data_);
			
			if(!mess.startsWith("DESC"))
			{
				System.out.println("Le diffuseur n'a pas renvoyé DESC, echec");
				sock.close();
				return;
			}
			System.out.println("Informations sur la radio ->	----- " + nom_propre(mess).substring(5) + " ------");
			
			boolean passe = false;
			sock.getInputStream().read(data,0,6);
			mess = new String(data);
			while(!mess.startsWith("ENDM"))
			{
				if(mess.startsWith("VOTE"))
					ecrireHistoire = true;
				else if(mess.startsWith("DOWN"))
					downloadHistoire = true;
				else if(mess.startsWith("PLAY"))
					play = true;
				else
				{
					System.out.println("La radio n'a pas renvoyé le bon message... : " + mess);
					passe = true;
				}
				if(!passe)
					instr.add(mess.substring(0,4));
				sock.getInputStream().read(data_,0,6);
				mess = new String(data_);
			}
			sock.close();
			if(Client.verbose)
				System.out.println("/////------------ Instructions Diffuseurs réalisée ------------/////");
		}
		catch (Exception e)
		{
			System.out.println("Nous ne pouvons pas vous connecter sur le diffuseur pour le moment. (demandeInfoDiffuseur)");
		}
		
		System.out.println("\nVous pouvez maintenant accèder à ces commande dans le menu principal: ");
		for(String ins: instr)
			afficherInstr(addr_diffuseurTCP, port_diffuseurTCP, ins);
	}

	public void afficherInstr(String addr_diffuseurTCP, int port_diffuseurTCP, String instr)
	{
		if(Client.verbose)
			System.out.println("/////------------ Afficher Instructions... ------------/////");
		byte[] data_ = new byte[148];
		try
		{
			Socket sock = new Socket(addr_diffuseurTCP, port_diffuseurTCP);	
			PrintWriter pw_ = new PrintWriter(new OutputStreamWriter(sock.getOutputStream()));
			
			String message = "HELP "+instr+"\r\n";
			pw_.write(message);
			pw_.flush();
			
			sock.getInputStream().read(data_,0,148);
			String mess = new String(data_);
			System.out.println("-> " + nom_propre(mess).substring(5));
			sock.close();
		}
		catch (Exception e)
		{
			e.printStackTrace();
			System.out.println("Nous ne pouvons pas vous connecter sur le diffuseur pour le moment. (demandeInfoDiffuseur)");
		}
		if(Client.verbose)
			System.out.println("/////------------ Instructions affichée ------------/////");
	}
	
	public static void suppressionRadio()
	{
		if(Client.verbose)
			System.out.println("/////------------ Demande de suppression d'une radio... ------------/////");
		System.out.println("\n ---------------------------------------------------");
		System.out.println("| Quelle radio voulez-vous arreter d'ecouter ?");
		String nom = quelDiffuseur();
		Client.tableau_diffuseurs_choisis.get(nom).eteindreThread();
		Client.tableau_diffuseurs_choisis.remove(nom);
		
		if(Client.verbose)
			System.out.println("/////------------ Radio "+nom+" supprimee ------------/////");
	}
}
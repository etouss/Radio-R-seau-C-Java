import java.io.BufferedWriter;
import java.io.FileWriter;
import java.io.IOException;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.net.DatagramPacket;
import java.net.InetAddress;
import java.net.MulticastSocket;
import java.net.Socket;
import java.util.HashMap;
import java.util.Arrays;

public class EcouterDiffuseur implements Runnable
{
	// Tous les elements servant a la diffusion/reception de messages
	private String nom_terminal;
	private String nom_diffuseur;
	private String addr_diffuseurTCP;
	private String addr_diffuseurUDP;
	private int port_diffuseurTCP;
	private int port_diffuseurUDP;
	private Thread thread_courant;

	// Constructeur
	public EcouterDiffuseur(String _nom_terminal, String _nom_diffuseur, String _addr_diffuseurTCP, String _addr_diffuseurUDP, int _port_diffuseurUDP, int _port_diffuseurTCP)
	{
		nom_terminal = _nom_terminal;
		addr_diffuseurTCP = _addr_diffuseurTCP;
		addr_diffuseurUDP = _addr_diffuseurUDP;
		port_diffuseurTCP = _port_diffuseurTCP;
		port_diffuseurUDP = _port_diffuseurUDP;
		nom_diffuseur = _nom_diffuseur;
	}

	public String nom_menu(String nom)
	{
		String result = nom;
		while (result.length() < 8)
			result += ' ';

		return result;
	}

	public void print_mess(String [] message,PrintWriter pw){
		if (!MenuDiffuseur.nom_propre(message[0]).equals(nom_diffuseur))
		{
			String nom = messagePropre(message[0]);
			pw.println("\n\n\n -------------------------------------------------------");
			pw.println("| On a un message de l'utilisateur " + nom + " pour vous !");
			pw.println("\t -> " + MenuDiffuseur.nom_propre(message[1]));
			pw.println("| Merci à toi " + nom + " a bientot");
			pw.println(" -------------------------------------------------------\n\n\n");
		}
		// Sinon c'est un message du diffuseur courant et on ecrit normal
		else
			pw.print(messagePropre(message[1]));
		pw.flush(); 
	}

	// Methode au lancement du Thread
	public void run()
	{
		try
		{
			int precedent = -1;
			if (Client.verbose)
				System.out.println("/////------------ Thread lance ------------/////");
			// On lie notre programme au terminal ou l'on va diffusee la radio
			PrintWriter pw = new PrintWriter(new BufferedWriter(new FileWriter(nom_terminal)));

			if (Client.verbose)
				System.out.println("/////------------ Ecriture mode verbose dans le terminal " + nom_terminal + " ------------/////");

			if (Client.verbose)
			{
				pw.println("/////------------ Ouverture terminal reussie, connexion au diffuseur... ------------/////");
				pw.flush();
			}
			// La socket d'envoye de paquet
			byte[] data = new byte[162];
			MulticastSocket mso = new MulticastSocket(port_diffuseurUDP);
			mso.joinGroup(InetAddress.getByName(addr_diffuseurUDP));
			DatagramPacket paquet = new DatagramPacket(data, data.length);

			if (Client.verbose)
			{
				pw.println("/////------------ Connexion au diffuseur reussie ------------/////");
				pw.flush();
			}

			// Welcome
			pw.println("\n\n\n -------------------------------------------------------");
			pw.println("|                                                       |");
			pw.println("| Bonjour ! Bienvenue sur votre radio preferee " + nom_menu(nom_diffuseur) + " |");
			pw.println("|                                                       |");
			pw.println(" -------------------------------------------------------");
			pw.flush();

			if (Client.verbose)
			{
				pw.println("/////------------ Lancement de l'ecoute ------------/////");
				pw.flush();
			}
			// On ecoute la radio
			while (!thread_courant.isInterrupted())
			{
				if (Client.verbose)
				{
					pw.println("/////------------ Reception paquet------------/////");
					pw.flush();
				}
				mso.receive(paquet);
				String message_recu = new String(paquet.getData(), 0, paquet.getLength()); // [DIFF num-mess id message]
				String splits[] = message_recu.split(" ", 4); // 4 pour ne pas split les espaces appartenants au message
				int id_mess = Integer.parseInt(splits[1]);

				// Cas ou des messages non pas ete reçus
				if (precedent == -1 || id_mess == precedent + 1 || (id_mess == 0 && precedent == 9999))
				{
					//cas de base
					if (Client.verbose)
						System.out.println("/////------------ Aucun message manquant ------------/////" + precedent + ":" +id_mess);
					print_mess(new String[] {splits[2],messagePropre(splits[3])},pw);
					precedent = id_mess;

				}
				else if (id_mess > precedent)
				{
					System.out.println("/////------------ Messages manquant ------------/////"+ precedent + ":" +id_mess);
					int tmp = precedent + 1;

					HashMap<Integer, String[]> tab = recuperationMessagesPerdus(id_mess, precedent, (id_mess - precedent - 1));
					Integer [] tmp_2 = tab.keySet().toArray(new Integer[tab.keySet().size()]);
					Arrays.sort(tmp_2);
					for (int index : tmp_2)
					{
						if (index == (tmp))
						{
							print_mess(tab.get(index),pw);
						}
						tmp++;
					}
					precedent = tmp-1;
				} 
				// Si l'entite change, c'est un utilisateur qui ecrit et on le fait savoir

			}
			if (Client.verbose)
			{
				pw.println("/////------------ Thread fermé, fermeture du diffuseur sur le terminal... ------------/////");
				pw.flush();
			}
			pw.println("\n\n\n -------------------------------------------------------");
			pw.println("| Merci de nous avoir écouté, à bientôt sur " + nom_menu(nom_diffuseur) + "    |");
			pw.println("|                   Fin de la connexion                 |");
			pw.println(" -------------------------------------------------------\n\n");
			pw.flush();

			pw.close();
			mso.close();
		}
		catch (IOException e)
		{
			System.out.println("Erreur sur l'écoute UDP");
		}
	}

	public HashMap<Integer, String[]> recuperationMessagesPerdus(int id_mess, int precedent, int diff)
	{
		HashMap<Integer, String[]> tab = new HashMap<Integer, String[]>();

		try
		{
			Socket sock = new Socket(addr_diffuseurTCP, port_diffuseurTCP);
			PrintWriter pw = new PrintWriter(new OutputStreamWriter(sock.getOutputStream()));

			// Demande au diffuseur des nb derniers messages
			pw.print("LAST " + (diff+10) + "\r\n");
			pw.flush();

			while (true)
			{
				byte[] data = new byte[161];
				sock.getInputStream().read(data, 0, 161);
				String mess = new String(data);

				// Message stipulant que le diffuseur a fini d'envoyer
				if (!mess.startsWith("ENDM"))
				{
					/* System.out.println(mess); */
					String[] message = mess.split(" ", 4);
					int id = Integer.parseInt(message[1]);
					if ( id > precedent)
					{
						tab.put(id, new String[]{message[2],message[3]});
						continue;
					}

				}
				break;
			}
			pw.close();
			sock.close();
		}
		catch (Exception e)
		{
			e.printStackTrace();
			System.out.println("Nous ne pouvons pas vous connecter sur le diffuseur pour le moment. (Messages perdus)");
		}

		return tab;
	}

	public String messagePropre(String message)
	{
		String result = "";
		for (char c : message.toCharArray())
		{
			if (c != '#')
				result += c;
		}
		return result;
	}

	public int getPortTCP()
	{
		return port_diffuseurTCP;
	}

	public String getAddrTCP()
	{
		return addr_diffuseurTCP;
	}

	public void eteindreThread()
	{
		if (Client.verbose)
			System.out.println("/////------------ Demande de fermeture du thread ------------/////");
		thread_courant.interrupt();
	}

	public void setThread_courant(Thread thread_courant)
	{
		this.thread_courant = thread_courant;
	}
}

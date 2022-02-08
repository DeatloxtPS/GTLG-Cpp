﻿#pragma warning (disable: 4018)
#pragma warning (disable: 4244)
#pragma warning (disable: 4286)
#include "SendChat.h"
#include "load_file.cpp"
#include "save.h"

// Don't rebuild project if it gives a build error relating . . ./bin

int main()
{
	Console_Handler();
	load_files();
	enet_initialize();
	cout << "ProjectL Version 1.04" << endl; cout << "ProjectL running on GTLG" << endl; cout << "Built-in HTTP: Enabled" << endl; cout << "" << endl; cout << "-----------------------------  " << endl; cout << "" << endl; cout << "Incoming Warnings/Errors:" << endl;

	ENetAddress address;
	enet_address_set_host
	(
		&address,
		"0.0.0.0"
	);
	address.port = configPort;
	server = enet_host_create
	(
		&address,
		50,
		10,
		0,
		0
	);
	if (server == nullptr)
	{
		ColoredText("Major Error: Port 80 or 17091 is already in use!", "red");
		system("PAUSE");
		return -1;
	}
	server->checksum = enet_crc32;
	enet_host_compress_with_range_coder(server);
	buildItemsDatabase();
	if (autosave) threads.push_back(std::thread(save_manager));
	ENetEvent event;
	while (true)
	{
		while (enet_host_service
		(
			server,
			&event,
			1000
		)) {
			switch (event.type)
			{
			case ENET_EVENT_TYPE_CONNECT:

				event.peer->data = new PlayerInfo;
				sendData(event.peer, 1, nullptr, 0);
				break;

			case ENET_EVENT_TYPE_RECEIVE:
			{
				ENetPeer* peer = event.peer;
				if (peer->data == NULL) continue;
				PlayerInfo* pData = GetPeerData(peer);
				int messageType = GetMessageTypeFromPacket(event.packet);
				WorldInfo* world = worldDB.get_pointer(pData->currentWorld);
				if (world == NULL) continue;
				switch (messageType) {
				case 2:
				{
					string cch = GetTextPointerFromPacket(event.packet);
					if (cch.size() > 2048 || cch.size() <= 3) break;

					if (!pData->passed_data_check)
					{
						try {
							stringstream ss(GetTextPointerFromPacket(event.packet));
							string to = "";
							while (std::getline(ss, to, '\n')) {
								if (to.find('|') == -1) continue;
								string id = to.substr(0, to.find("|"));
								string act = to.substr(to.find("|") + 1, to.length() - to.find("|") - 1);
								if (id == "tankIDName") {
									if (act.length() > 25) break;
									pData->tankIDName = act;
									pData->haveGrowId = true;
								}
								else if (id == "tankIDPass")
								{
									pData->tankIDPass = act;
								}
								else if (id == "requestedName")
								{
									pData->requestedName = act;
								}
								else if (id == "country")
								{
									pData->country = act;
								}
								char clientConnection[16];
								enet_address_get_host_ip(&peer->address, clientConnection, 16);
								pData->charIP = clientConnection;
							}
							// loop
							gamepacket_t p;
							p.Insert("OnSuperMainStartAcceptLogonHrdxs47254722215a");
							p.Insert(itemdathash); p.Insert(server_ip); p.Insert("cache/");
							p.Insert("cc.cz.madkite.freedom org.aqua.gg idv.aqua.bulldog com.cih.gamecih2 com.cih.gamecih com.cih.game_cih cn.maocai.gamekiller com.gmd.speedtime org.dax.attack com.x0.strai.frep com.x0.strai.free org.cheatengine.cegui org.sbtools.gamehack com.skgames.traffikrider org.sbtoods.gamehaca com.skype.ralder org.cheatengine.cegui.xx.multi1458919170111 com.prohiro.macro me.autotouch.autotouch com.cygery.repetitouch.free com.cygery.repetitouch.pro com.proziro.zacro com.slash.gamebuster");
							p.Insert("proto=156|choosemusic=audio/mp3/about_theme.mp3|active_holiday=13|wing_week_day=0|ubi_week_day=0|server_tick=125378326|clash_active=0|drop_lavacheck_faster=1|isPayingUser=0|usingStoreNavigation=1|enableInventoryTab=1|bigBackpack=1|");
							p.CreatePacket(peer);
							if (pData->haveGrowId)
							{
								int logStatus = PlayerDB::playerLogin(peer, pData->tankIDName, pData->tankIDPass);
								switch (logStatus)
								{
								case -3:
								{
									FailLogin(peer, "action|log\nmsg|`4Sorry, this account (`5" + PlayerDB::getProperName(pData->tankIDName) + "`4) has been suspended. Contact discord `6@Developers ``if you have any questions.", false);
									break;
								}
								case -6:
								{
									FailLogin(peer, "action|log\nmsg|`4Advanced Account Protection: `oYou tried to log in from the new Device and IP. A verification email was sent to the email address registered with this GrowID (" + pData->email + "). Please follow the link in that email to whitelist this device and IP.", false);
									break;
								}
								case -7:
								{
									FailLogin(peer, "action|log\nmsg|`oServer is being worked on, `4maintenance `ois gonna take a few minutes!", true);
									break;
								}
								case -8:
								{
									FailLogin(peer, "action|log\nmsg|`4Sorry, this account, device or location has been temporarily suspended.<CR>`oIf you didn't do anything wrong, it could be because you're playing from the same place or on the same device as someone who did. Contact `6@Developers `oif you have any questions. This is a temporary ban caused by `w" + pData->rawName + " `oand will be removed in `w" + OutputBanTime(calcBanDuration(pData->timeBanned)) + "`o. If that's not your name, try playing from another location or device to fix it.", false);
									break;
								}
								case -9:
								{
									FailLogin(peer, "action|log\nmsg|`4Sorry, this device or location is perma banned.", false);
									break;
								}
								case -10:
								{
									break;
								}
								case -1:
								{
									FailLogin(peer, "action|log\nmsg|`4Unable to log on: `oThat `wGrowID `odoesn't seem valid`w, `oor the password is wrong`w. `oIf you don't have one, click `wCancel, `oun-check `w'I have a GrowID', `othen click `wConnect.", true);
									break;
								}
								case 1:
								{
									pData->HasLogged = true;
									break;
								}
								default:
								{
									break;
								}
								}
							}
							pData->passed_data_check = true;
						}
						catch (const std::out_of_range& e)
						{
							std::cout << e.what() << std::endl;
						}
						break;
					}
					//

				
					

					


					if (cch == "action|enter_game\n" && pData->passed_data_check) {
						if (!pData->haveGrowId)
						{
							pData->rawName = "" + PlayerDB::fixColors(pData->requestedName.substr(0, pData->requestedName.length() > 15 ? 15 : pData->requestedName.length()) + "_" + to_string(rand() % 1000));
							pData->msgName = std::to_string(peer->address.host);
							pData->displayName = PlayerDB::fixColors(pData->requestedName.substr(0, pData->requestedName.length() > 15 ? 15 : pData->requestedName.length()) + "_" + to_string(rand() % 1000));
							pData->tankIDName = PlayerDB::fixColors(pData->requestedName.substr(0, pData->requestedName.length() > 15 ? 15 : pData->requestedName.length()) + "_" + to_string(rand() % 1000));
							pData->displayNamebackup = pData->displayName;
							string lower = pData->rawName;
							std::transform(lower.begin(), lower.end(), lower.begin(), ::toupper);
							checkIpBan(peer);
						}
						else
						{
							if (!pData->HasLogged)
							{
								break;
							}
							LoadPlayerData(peer);
						}
						if (pData->country.length() > 4)
						{
							pData->country = "us";
						}
						if (pData->haveGrowId)
						{
							GamePacket p9 = packetEnd(appendInt(appendString(createPacket(), "SetHasAccountSecured"), 1));
							ENetPacket* packet9 = enet_packet_create(p9.data, p9.len, ENET_PACKET_FLAG_RELIABLE);
							enet_peer_send(peer, 0, packet9);
							delete p9.data;
							GamePacket p36 = packetEnd(appendString(appendString(appendInt(appendString(createPacket(), "SetHasGrowID"), 1), pData->tankIDName), pData->tankIDPass));
							ENetPacket* packet36 = enet_packet_create(p36.data, p36.len, ENET_PACKET_FLAG_RELIABLE);
							enet_peer_send(peer, 0, packet36);
							delete p36.data;
						}
						else
						{
							GamePacket p42 = packetEnd(appendString(appendString(appendInt(appendString(createPacket(), "SetHasGrowID"), 0), ""), ""));
							ENetPacket* packet42 = enet_packet_create(p42.data, p42.len, ENET_PACKET_FLAG_RELIABLE);
							enet_peer_send(peer, 0, packet42);
							delete p42.data;
						}
						pData->isIn = true;
						GamePacket p2ssw = packetEnd(appendString(appendInt(appendString(createPacket(), "OnEmoticonDataChanged"), 201560520), "(wl)|Ä|1&(yes)|Ä‚|1&(no)|Äƒ|1&(love)|Ä„|1&(oops)|Ä…|1&(shy)|Ä†|1&(wink)|Ä‡|1&(tongue)|Äˆ|1&(agree)|Ä‰|1&(sleep)|ÄŠ|1&(punch)|Ä‹|1&(music)|ÄŒ|1&(build)|Ä|1&(megaphone)|ÄŽ|1&(sigh)|Ä|1&(mad)|Ä|1&(wow)|Ä‘|1&(dance)|Ä’|1&(see-no-evil)|Ä“|1&(bheart)|Ä”|1&(heart)|Ä•|1&(grow)|Ä–|1&(gems)|Ä—|1&(kiss)|Ä˜|1&(gtoken)|Ä™|1&(lol)|Äš|1&(smile)|Ä€|1&(cool)|Äœ|1&(cry)|Ä|1&(vend)|Äž|1&(bunny)|Ä›|1&(cactus)|ÄŸ|1&(pine)|Ä¤|1&(peace)|Ä£|1&(terror)|Ä¡|1&(troll)|Ä¢|1&(evil)|Ä¢|1&(fireworks)|Ä¦|1&(football)|Ä¥|1&(alien)|Ä§|1&(party)|Ä¨|1&(pizza)|Ä©|1&(clap)|Äª|1&(song)|Ä«|1&(ghost)|Ä¬|1&(nuke)|Ä­|1&(halo)|Ä®|1&(turkey)|Ä¯|1&(gift)|Ä°|1&(cake)|Ä±|1&(heartarrow)|Ä²|1&(lucky)|Ä³|1&(shamrock)|Ä´|1&(grin)|Äµ|1&(ill)|Ä¶|1&"));
						ENetPacket* packet2ssw = enet_packet_create(p2ssw.data, p2ssw.len, ENET_PACKET_FLAG_RELIABLE);
						enet_peer_send(peer, 0, packet2ssw);
						delete p2ssw.data;
						if (pData->haveGrowId)
						{
							string name = pData->displayName;
							int onlinecount = 0;
							int totalcount = pData->friendinfo.size();
							for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer)
							{
								if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
								if (find(pData->friendinfo.begin(), pData->friendinfo.end(), static_cast<PlayerInfo*>(currentPeer->data)->rawName) != pData->friendinfo.end())
								{
									onlinecount++;
								}
							}
							if (onlinecount == 0) {
								Player::OnConsoleMessage(peer, "Welcome back, `w" + name + "`o. No friends are online.");
							}
							else {
								Player::OnConsoleMessage(peer, "Welcome back, `w" + name + "`o. `w" + to_string(onlinecount) + "`o friend is online.");
							}
							Player::OnConsoleMessage(peer, "`oWhere would you like to go? (`w" + GetPlayerCountServer() + " `oonline)");
							if (gem_multiplier != 0) {
								Player::OnConsoleMessage(peer, "`5GTLG Events`o: `2Gem multiplier (" + to_string(gem_multiplier) + "x)");
							}
							else {
								Player::OnConsoleMessage(peer, "`5GTLG Events`o: No events going on...");
							}
							GamePacket p36 = packetEnd(appendString(appendString(appendInt(appendString(createPacket(), "SetHasGrowID"), 1), pData->tankIDName), pData->tankIDPass));
							ENetPacket* packet36 = enet_packet_create(p36.data, p36.len, ENET_PACKET_FLAG_RELIABLE);
							enet_peer_send(peer, 0, packet36);
							delete p36.data;
							GamePacket p9 = packetEnd(appendInt(appendString(createPacket(), "SetHasAccountSecured"), 1));
							ENetPacket* packet9 = enet_packet_create(p9.data, p9.len, ENET_PACKET_FLAG_RELIABLE);
							enet_peer_send(peer, 0, packet9);
							delete p9.data;
							GamePacket p12 = packetEnd(appendInt(appendInt(appendString(createPacket(), "OnTodaysDate"), 2), 6));
							ENetPacket* packet12 = enet_packet_create(p12.data, p12.len, ENET_PACKET_FLAG_RELIABLE);
							enet_peer_send(peer, 0, packet12);
							delete p12.data;
							GamePacket p19 = packetEnd(appendInt(appendString(createPacket(), "SetShowChatOnlyFromFriends"), 0));
							ENetPacket* packet19 = enet_packet_create(p19.data, p19.len, ENET_PACKET_FLAG_RELIABLE);
							enet_peer_send(peer, 0, packet19);
							delete p19.data;
							updateplayerset(peer, pData->cloth_hand);
							updateplayerset(peer, pData->cloth_face);
							updateplayerset(peer, pData->cloth_mask);
							updateplayerset(peer, pData->cloth_shirt);
							updateplayerset(peer, pData->cloth_ances);
							updateplayerset(peer, pData->cloth_pants);
							updateplayerset(peer, pData->cloth_necklace);
							updateplayerset(peer, pData->cloth_feet);
							updateplayerset(peer, pData->cloth_back);
							if (pData->haveGrowId) {
								if (pData->lastworld != "EXIT" && pData->lastworld != "") {
									handle_world(peer, pData->lastworld, true);
								}
								else {
									sendWorldOffers(peer);
								}
							}
							ifstream news("config+/server_config/news.txt");
							stringstream buffer;
							buffer << news.rdbuf();
							string newsString(buffer.str());
							Player::OnDialogRequest(peer, newsString);
							bool iscontains = false, success = true;
							SearchInventoryItem(peer, 18, 1, iscontains);
							if (!iscontains) SaveItemMoreTimes(18, 1, peer, success);
							SearchInventoryItem(peer, 32, 1, iscontains);
							if (!iscontains) SaveItemMoreTimes(32, 1, peer, success);
						}
						else {
							PlayerInventory inventory;
							InventoryItem item{};
							item.itemCount = 1;
							item.itemID = 18;
							inventory.items.push_back(item);
							item.itemCount = 1;
							item.itemID = 32;
							inventory.items.push_back(item);
							SendInventory(peer, inventory);
							pData->inventory = inventory;
							handle_world(peer, "START");
						} for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
							if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
							string name = static_cast<PlayerInfo*>(currentPeer->data)->rawName;
							if (find(pData->friendinfo.begin(), pData->friendinfo.end(), name) != pData->friendinfo.end()) {
								Player::OnConsoleMessage(currentPeer, "`3FRIEND ALERT: `o" + pData->displayName + " `ohas `2logged on`o.");
							}
						}
						break;
					}
					if (cch == "action|refresh_item_data\n") {
						if (itemsDat != NULL) {
							ENetPacket* packet = enet_packet_create(itemsDat, static_cast<size_t>(itemsDatSize) + 60, ENET_PACKET_FLAG_RELIABLE);
							enet_peer_send(peer, 0, packet);
							pData->isUpdating = true;
						}
						break;
					}
					else if (cch == "action|claimprogressbar\n") {
						if (ValentineEvent)
						{
							if (pData->bootybreaken >= 100) {
								Player::OnDialogRequest(peer, "set_default_color|`o\nadd_label_with_icon|big|`wAbout Valentine's Event``|left|384|\nadd_spacer|small|\nadd_textbox|During Valentine's Week you will gain points for opening Golden Booty Chests. Claim enough points to earn bonus rewards.|left|\nadd_spacer|small|\nadd_textbox|Current Progress: " + to_string(pData->bootybreaken) + "/100|left|\nadd_spacer|small|\nadd_textbox|Reward:|left|\nadd_label_with_icon|small|Super Golden Booty Chest|left|9350|\nadd_smalltext|             - 4x chance of getting a Golden Heart Crystal when opening!|left|\nadd_spacer|small|\nadd_button|claimreward|Claim Reward|no_flags|0|0|\nend_dialog|valentines_quest||OK|");
							}
							else {
								Player::OnDialogRequest(peer, "set_default_color|`o\nadd_label_with_icon|big|`wAbout Valentine's Event``|left|384|\nadd_spacer|small|\nadd_textbox|During Valentine's Week you will gain points for opening Golden Booty Chests. Claim enough points to earn bonus rewards.|left|\nadd_spacer|small|\nadd_textbox|Current Progress: " + to_string(pData->bootybreaken) + "/100|left|\nadd_spacer|small|\nadd_textbox|Reward:|left|\nadd_label_with_icon|small|Super Golden Booty Chest|left|9350|\nadd_smalltext|             - 4x chance of getting a Golden Heart Crystal when opening!|left|\nend_dialog|valentines_quest||OK|");
							}
						}
						break;
					}
					else if (cch.find("action|wrench") == 0)
					{
						try
						{
							if (pData->isCursed)
							{
								Player::OnTextOverlay(peer, "`wCan't wrench when your `4cursed`w!");
								continue;
							}
							if (pData->trade) end_trade(peer);
							std::stringstream ss(cch);
							std::string to;
							int id = -1;
							while (std::getline(ss, to, '\n'))
							{
								vector<string> infoDat = explode("|", to);
								if (infoDat.at(1) == "netid")
								{
									id = atoi(infoDat.at(2).c_str());
								}
							}
							if (id < 0) continue;
							for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
								if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
								if (isHere(peer, currentPeer)) {
									if (static_cast<PlayerInfo*>(currentPeer->data)->netID == id) {
										pData->lastInfo = static_cast<PlayerInfo*>(currentPeer->data)->rawName;
										pData->lastInfoname = static_cast<PlayerInfo*>(currentPeer->data)->tankIDName;
										pData->lastDisplayname = static_cast<PlayerInfo*>(currentPeer->data)->displayName;
										string name = static_cast<PlayerInfo*>(currentPeer->data)->displayName;
										string rawnam = pData->rawName;
										string rawnamofwrench = static_cast<PlayerInfo*>(currentPeer->data)->rawName;
										string guildlabel = "";
										string levellabel = "";
										if (static_cast<PlayerInfo*>(currentPeer->data)->guild == "" || mod(currentPeer) && static_cast<PlayerInfo*>(currentPeer->data)->isNicked == false) {
											guildlabel = "";
										}
										else {
											string guild_rank = "Member";
											switch (static_cast<PlayerInfo*>(currentPeer->data)->guild_rank) {
											case 1:
											{
												guild_rank = "Elder";
												break;
											}
											case 2:
											{
												guild_rank = "Co-Leader";
												break;
											}
											case 3:
											{
												guild_rank = "Leader";
												break;
											}
											default:
											{
												break;
											}
											}
											if (static_cast<PlayerInfo*>(currentPeer->data)->guildFg != 0 && static_cast<PlayerInfo*>(currentPeer->data)->guildBg != 0) {
												guildlabel = "\nadd_dual_layer_icon_label|small|`9Guild: `2" + static_cast<PlayerInfo*>(currentPeer->data)->guild + "``|left|" + std::to_string(static_cast<PlayerInfo*>(currentPeer->data)->guildBg) + "|" + std::to_string(static_cast<PlayerInfo*>(currentPeer->data)->guildFg) + "|1.0|1|\nadd_textbox|`9Rank: `2" + guild_rank + "``|left|\nadd_spacer|small|";
											}
											else {
												guildlabel = "\nadd_label_with_icon|small|`9Guild: `2" + static_cast<PlayerInfo*>(currentPeer->data)->guild + "``|left|242|\nadd_textbox|`9Rank: `2" + guild_rank + "``|left|\nadd_spacer|small|";
											}
										}
										if (mod(currentPeer) && static_cast<PlayerInfo*>(currentPeer->data)->isNicked == false) {
											levellabel = "?";
										}
										else {
											levellabel = to_string(static_cast<PlayerInfo*>(currentPeer->data)->level);
										}
										string guilddialog = "";
										if (pData->guild != "" && pData->guild_rank > 0) guilddialog = "\nadd_button|inviteguildbutton|`2Invite to Guild|0|0|";
										if (rawnamofwrench != rawnam) {
											if (rawnamofwrench != "") {
												if (!restricted_area(peer, world, static_cast<PlayerInfo*>(currentPeer->data)->x / 32, static_cast<PlayerInfo*>(currentPeer->data)->y / 32) && world->owner == "" || world->owner == pData->rawName && pData->haveGrowId || mod(peer)) {
													if (mod(peer)) {
														Player::OnDialogRequest(peer, "set_default_color|`o\nadd_label_with_icon|big|`w" + name + " `w(`2" + levellabel + "`w)``|left|18|\nadd_spacer|small|" + guildlabel + "\nadd_button|punishview|`wPunish/View|0|0|\nadd_button|trade|`wTrade``|noflags|0|0|\nadd_textbox|(No Battle Leash equipped)|left|\nadd_textbox|Your opponent needs a valid license to battle!|left|\nadd_button|kick|`6Kick``|noflags|0|0|\nadd_button|pull|`2Pull``|noflags|0|0|\nadd_button|wban|`4World Ban``|noflags|0|0|" + guilddialog + "\nadd_button|addfriendrnbutton|`wAdd as friend``|noflags|0|0|\nadd_button|ignore_player|`wIgnore Player``|noflags|0|0|\nadd_button|report_player|`wReport Player``|noflags|0|0|\nadd_spacer|small|\nend_dialog|popup||Continue|\nadd_quick_exit|");
													}
													else {
														if (!restricted_area(peer, world, static_cast<PlayerInfo*>(currentPeer->data)->x / 32, static_cast<PlayerInfo*>(currentPeer->data)->y / 32) && world->owner == "") {
															Player::OnDialogRequest(peer, "set_default_color|`o\nadd_label_with_icon|big|`w" + name + " `w(`2" + levellabel + "`w)``|left|18|\nadd_spacer|small|" + guildlabel + "\nadd_button|trade|`wTrade``|noflags|0|0|\nadd_textbox|(No Battle Leash equipped)|left|\nadd_textbox|Your opponent needs a valid license to battle!|left|\nadd_button|kick|`6Kick``|noflags|0|0|\nadd_button|pull|`2Pull``|noflags|0|0|" + guilddialog + "\nadd_button|addfriendrnbutton|`wAdd as friend``|noflags|0|0|\nadd_button|ignore_player|`wIgnore Player``|noflags|0|0|\nadd_button|report_player|`wReport Player``|noflags|0|0|\nadd_spacer|small|\nend_dialog|popup||Continue|\nadd_quick_exit|");
														}
														else {
															Player::OnDialogRequest(peer, "set_default_color|`o\nadd_label_with_icon|big|`w" + name + " `w(`2" + levellabel + "`w)``|left|18|\nadd_spacer|small|" + guildlabel + "\nadd_button|trade|`wTrade``|noflags|0|0|\nadd_textbox|(No Battle Leash equipped)|left|\nadd_textbox|Your opponent needs a valid license to battle!|left|\nadd_button|kick|`6Kick``|noflags|0|0|\nadd_button|pull|`2Pull``|noflags|0|0|\nadd_button|wban|`4World Ban``|noflags|0|0|" + guilddialog + "\nadd_button|addfriendrnbutton|`wAdd as friend``|noflags|0|0|\nadd_button|ignore_player|`wIgnore Player``|noflags|0|0|\nadd_button|report_player|`wReport Player``|noflags|0|0|\nadd_spacer|small|\nend_dialog|popup||Continue|\nadd_quick_exit|");
														}
													}
												}
												else {
													Player::OnDialogRequest(peer, "set_default_color|`o\nadd_label_with_icon|big|`w" + name + " `w(`2" + levellabel + "`w)``|left|18|\nadd_spacer|small|" + guildlabel + "\nadd_button|trade|`wTrade``|noflags|0|0|\nadd_textbox|(No Battle Leash equipped)|left|\nadd_textbox|Your opponent needs a valid license to battle!|left|" + guilddialog + "\nadd_button|addfriendrnbutton|`wAdd as friend``|noflags|0|0|\nadd_button|ignore_player|`wIgnore Player``|noflags|0|0|\nadd_button|report_player|`wReport Player``|noflags|0|0|\nadd_spacer|small|\nend_dialog|popup||Continue|\nadd_quick_exit|");
												}
											}
										}
										else {
											if (pData->haveGrowId == true) {
												send_info(peer, pData);
											}
											else
											{
												SendRegisterDialog(peer);
												enet_host_flush(server);
											}
										}
									}
								}
							}
						}
						catch (const std::out_of_range& e) {
							std::cout << e.what() << std::endl;
						}
						break;
					}
					else if (cch.find("action|setSkin") == 0) {
						if (!world) continue;
						try {
							std::stringstream ss(cch);
							std::string to;
							int id = -1;
							string color = "";
							while (std::getline(ss, to, '\n')) {
								vector<string> infoDat = explode("|", to);
								if (infoDat.at(0) == "color") color = infoDat.at(1);
								if (has_only_digits(color) == false) continue;
								id = atoi(color.c_str());
								if (color == "2190853119") id = -2104114177;
								else if (color == "2527912447") id = -1767054849;
								else if (color == "2864971775") id = -1429995521;
								else if (color == "3033464831") id = -1261502465;
								else if (color == "3370516479") id = -924450817;
							}
							pData->skinColor = id;
							sendClothes(peer);
						}
						catch (const std::out_of_range& e) {
							std::cout << e.what() << std::endl;
						}
						break;
					}
					else if (cch == "action|respawn\n") {
						playerRespawn(world, peer, false);
						break;
					}
					else if (cch.find("action|respawn_spike") == 0) {
						playerRespawn(world, peer, false);
						break;
					}
					else if (cch.find("action|friends\n") == 0) {
						string GuildButtonDialog = "";
						if (pData->guild != "") {
							GuildButtonDialog = "\nadd_button|showguild|`wShow Guild Members``|noflags";
						}
						else {
							GuildButtonDialog = "\nadd_button|showguild|`wCreate Guild``|noflags";
						}
						Player::OnDialogRequest(peer, " set_default_color|`o\nadd_label_with_icon|big| `wSocial Portal`` |left|1366|\nadd_spacer|small|\nadd_button|showfriend|`wShow Friends``|noflags|0|0|\nadd_button|show_apprentices|`wShow Apprentices``|noflags|0|0|" + GuildButtonDialog + "|0|0|\nadd_button|communityhub|`wCommunity Hub``|noflags|0|0|\nadd_quick_exit|\nend_dialog|friends_guilds|OK||");
						break;
					}
					else if (cch == "action|growid\n") {
						SendRegisterDialog(peer);
						enet_host_flush(server);
						break;
					}
					else if (cch == "action|trade_cancel\n") {
						end_trade(peer, false, true);
						break;
					}
					else if (cch.find("action|trade_started\n") == 0) {
						try {
							std::stringstream ss(cch);
							std::string to;
							int id = -1;
							while (std::getline(ss, to, '\n')) {
								vector<string> infoDat = explode("|", to);
								if (infoDat.size() == 2) {
									if (infoDat.at(0) == "netid") {
										id = atoi(infoDat.at(1).c_str());
										break;
									}
								}
							}
							if (id == -1) continue;
							for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
								if (currentPeer->state != ENET_PEER_STATE_CONNECTED) continue;
								if (isHere(peer, currentPeer)) {
									if (static_cast<PlayerInfo*>(currentPeer->data)->netID == id) {
										if (pData->trade_netid == static_cast<PlayerInfo*>(currentPeer->data)->netID && static_cast<PlayerInfo*>(currentPeer->data)->trade_netid == pData->netID) {
											start_trade(peer, currentPeer);
											break;
										}
										Player::OnConsoleMessage(currentPeer, "`#TRADE ALERT:`` " + pData->displayName + " `owants to trade with you!  To start, use the `wWrench`` on that person's wrench icon, or type `w/trade " + pData->displayName + "``");
										Player::PlayAudio(currentPeer, "audio/cash_register.wav", 0);
										break;
									}
								}
							}
						}
						catch (const std::out_of_range& e) {
							std::cout << e.what() << std::endl;
						}
						break;
					}
					else if (cch.find("action|rem_trade\n") == 0) {
						try {
							std::stringstream ss(cch);
							std::string to;
							int id = -1;
							while (std::getline(ss, to, '\n')) {
								vector<string> infoDat = explode("|", to);
								if (infoDat.size() == 2) {
									if (infoDat.at(0) == "itemID") {
										id = atoi(infoDat.at(1).c_str());
										break;
									}
								}
							}
							if (id == -1) continue;
							for (int i = 0; i < pData->tradeItems.size(); i++) {
								if (pData->tradeItems.at(i).id == id) {
									pData->tradeItems.erase(pData->tradeItems.begin() + i);
									break;
								}
							}
							update_trade(peer, pData->trade_netid);
						}
						catch (const std::out_of_range& e) {
							std::cout << e.what() << std::endl;
						}
						break;
					}
					else if (cch.find("action|mod_trade\n") == 0) {
						try {
							std::stringstream ss(cch);
							std::string to;
							int id = -1;
							while (std::getline(ss, to, '\n')) {
								vector<string> infoDat = explode("|", to);
								if (infoDat.size() == 2) {
									if (infoDat.at(0) == "itemID") {
										id = atoi(infoDat.at(1).c_str());
										break;
									}
								}
							}
							if (id == -1 || id == 18) continue;
							if (pData->tradeItems.size() >= 4) {
								Player::PlayAudio(peer, "audio/cant_place_tile.wav", 0);
								break;
							}
							if (id == 6260 || getItemDef(id).properties & Property_Untradable || id == 18 || id == 32 || id == 6336 || id == 8552 || id == 9472 || id == 9482 || id == 9356 || id == 9492 || id == 9498 || id == 8774 || id == 1790 || id == 2592 || id == 1784 || id == 1792 || id == 1794 || id == 7734 || id == 8306 || id == 9458 || id == 5640) {
								if (!admin(peer) && !pData->Subscriber || id == 18 || id == 5640 || id == 32 || id == 6336 || id == 1486 || id == 1794) {
									Player::OnTextOverlay(peer, "You'd be sorry if you lost that!");
									Player::PlayAudio(peer, "audio/cant_place_tile.wav", 0);
									break;
								}
							}
							if (static_cast<PlayerInfo*>(peer->data)->lastdealchange + 1000 < (duration_cast<milliseconds>(system_clock::now().time_since_epoch())).count()) {
								static_cast<PlayerInfo*>(peer->data)->lastdealchange = (duration_cast<milliseconds>(system_clock::now().time_since_epoch())).count();
								int count = 0;
								for (auto i = 0; i < pData->inventory.items.size(); i++) {
									if (pData->inventory.items.at(i).itemID == id && pData->inventory.items.at(i).itemCount >= 1) {
										count = pData->inventory.items.at(i).itemCount;
										break;
									}
								}
								if (count == 1) {
									auto contains = false;
									SearchInventoryItem(peer, id, count, contains);
									if (!contains) break;
									for (int i = 0; i < pData->tradeItems.size(); i++) {
										if (pData->tradeItems.at(i).id == id) {
											pData->tradeItems.erase(pData->tradeItems.begin() + i);
											break;
										}
									}
									TradeItem trdItem = { id, count };
									pData->tradeItems.push_back(trdItem);
									update_trade(peer, pData->trade_netid);
									break;
								}
								Player::OnDialogRequest(peer, "set_default_color|`o\nadd_label_with_icon|big|`2Trade`` `w" + getItemDef(id).name + "``|left|" + to_string(id) + "|\nadd_textbox|`2Trade how many?``|left|\nadd_text_input|count_" + to_string(id) + "||" + to_string(count) + "|5|\nend_dialog|trade_add|Cancel|OK|");
							}
							else {
								Player::OnTextOverlay(peer, "Slow down!  Please wait a second between adding and removing items");
							}
						}
						catch (const std::out_of_range& e) {
							std::cout << e.what() << std::endl;
						}
						break;
					}
					else if (cch.find("action|trade_accept\n") == 0) {
						try {
							std::stringstream ss(cch);
							std::string to;
							int status = 0;
							while (std::getline(ss, to, '\n')) {
								vector<string> infoDat = explode("|", to);
								if (infoDat.size() == 2) {
									if (infoDat.at(0) == "status") {
										if (infoDat.at(1) == "1") {
											status = 1;
										}
										break;
									}
								}
							}
							if (!pData->trade) continue;
							pData->trade_accept = status;
							UpdateTradeAcceptedStatus(peer);
							for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
								if (currentPeer->state != ENET_PEER_STATE_CONNECTED) continue;
								if (isHere(peer, currentPeer)) {
									if (pData->trade_netid == ((PlayerInfo*)(currentPeer->data))->netID || ((PlayerInfo*)(peer->data))->netID == ((PlayerInfo*)(currentPeer->data))->trade_netid) {
										if (pData->trade_accept && static_cast<PlayerInfo*>(currentPeer->data)->trade_accept) {
											bool full_inv = false;
											int which_item = 0;
											if (static_cast<PlayerInfo*>(peer->data)->inventory.items.size() + static_cast<PlayerInfo*>(currentPeer->data)->tradeItems.size() >= static_cast<PlayerInfo*>(peer->data)->currentInventorySize) {
												Player::OnTextOverlay(peer, "`w" + static_cast<PlayerInfo*>(peer->data)->displayName + "`w needs more backpack room first!");
												Player::OnTextOverlay(currentPeer, "`w" + static_cast<PlayerInfo*>(peer->data)->displayName + "`w needs more backpack room first!");
												pData->trade_accept = false;
												static_cast<PlayerInfo*>(currentPeer->data)->trade_accept = false;
												UpdateTradeAcceptedStatus(peer);
												UpdateTradeAcceptedStatus(currentPeer);
												break;
											}
											for (auto& f : static_cast<PlayerInfo*>(currentPeer->data)->tradeItems) {
												if (CheckItemMaxed(peer, f.id, f.count) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && CheckItemExists(peer, f.id) && CheckItemMaxed(peer, f.id, f.count) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && !CheckItemExists(peer, f.id)) {
													full_inv = true;
													which_item = f.id;
													break;
												}
											}
											if (full_inv) {
												Player::OnTextOverlay(currentPeer, "`4Oops - " + pData->displayName + " `4is carrying too many " + getItemDef(which_item).name + " and can't fit that many in their backpack.");
												Player::OnTextOverlay(peer, "`4Oops - " + pData->displayName + " `4is carrying too many " + getItemDef(which_item).name + " and can't fit that many in their backpack.");
												pData->trade_accept = false;
												static_cast<PlayerInfo*>(currentPeer->data)->trade_accept = false;
												UpdateTradeAcceptedStatus(peer);
												UpdateTradeAcceptedStatus(currentPeer);
												break;
											}
											full_inv = false;
											if (static_cast<PlayerInfo*>(currentPeer->data)->inventory.items.size() + static_cast<PlayerInfo*>(peer->data)->tradeItems.size() >= static_cast<PlayerInfo*>(currentPeer->data)->currentInventorySize) {
												Player::OnTextOverlay(peer, "`w" + static_cast<PlayerInfo*>(currentPeer->data)->displayName + "`w needs more backpack room first!");
												Player::OnTextOverlay(currentPeer, "`w" + static_cast<PlayerInfo*>(currentPeer->data)->displayName + "`w needs more backpack room first!");
												pData->trade_accept = false;
												static_cast<PlayerInfo*>(currentPeer->data)->trade_accept = false;
												UpdateTradeAcceptedStatus(peer);
												UpdateTradeAcceptedStatus(currentPeer);
												break;
											}
											for (auto& f : static_cast<PlayerInfo*>(peer->data)->tradeItems) {
												if (CheckItemMaxed(currentPeer, f.id, f.count) || static_cast<PlayerInfo*>(currentPeer->data)->inventory.items.size() + 1 >= static_cast<PlayerInfo*>(currentPeer->data)->currentInventorySize && CheckItemExists(currentPeer, f.id) && CheckItemMaxed(currentPeer, f.id, f.count) || static_cast<PlayerInfo*>(currentPeer->data)->inventory.items.size() + 1 >= static_cast<PlayerInfo*>(currentPeer->data)->currentInventorySize && !CheckItemExists(currentPeer, f.id)) {
													full_inv = true;
													which_item = f.id;
													break;
												}
											}
											if (full_inv) {
												Player::OnTextOverlay(peer, "`4Oops - " + static_cast<PlayerInfo*>(currentPeer->data)->displayName + " `4is carrying too many " + getItemDef(which_item).name + " and can't fit that many in their backpack.");
												Player::OnTextOverlay(currentPeer, "`4Oops - " + static_cast<PlayerInfo*>(currentPeer->data)->displayName + " `4is carrying too many " + getItemDef(which_item).name + " and can't fit that many in their backpack.");
												pData->trade_accept = false;
												static_cast<PlayerInfo*>(currentPeer->data)->trade_accept = false;
												UpdateTradeAcceptedStatus(peer);
												UpdateTradeAcceptedStatus(currentPeer);
												break;
											}

											Player::OnForceTradeEnd(peer);
											Player::OnForceTradeEnd(currentPeer);
											string warning1 = "", warning2 = "", warningkeycurrent = "", warningkeypeer = "", lockedwith = "";
											if (static_cast<PlayerInfo*>(currentPeer->data)->tradeItems.size() == 0) {
												warning1 = "\nadd_spacer|small|\nadd_textbox|`4SCAM WARNING: ``You are about to do a trade without receiving anything in return. Once you do the trade you cannot get the items back.|left|\nadd_textbox|`4Do you really want to do this?``|left|\nadd_spacer|small|";
											}
											if (pData->tradeItems.size() == 0) {
												warning2 = "\nadd_spacer|small|\nadd_textbox|`4SCAM WARNING: ``You are about to do a trade without receiving anything in return. Once you do the trade you cannot get the items back.|left|\nadd_textbox|`4Do you really want to do this?``|left|\nadd_spacer|small|";
											}
											for (auto& f : static_cast<PlayerInfo*>(peer->data)->tradeItems) {
												if (f.id == 1424) {
													for (int i = 0; i < world->width * world->height; i++) {
														if (getItemDef(world->items.at(i).foreground).blockType == BlockTypes::LOCK) {
															if (world->items.at(i).foreground == 202 || world->items.at(i).foreground == 204 || world->items.at(i).foreground == 206 || world->items.at(i).foreground == 4994) continue;
															lockedwith = getItemDef(world->items.at(i).foreground).name;
															break;
														}
													}
													warningkeycurrent = "\nadd_textbox|`4SCAM WARNING: ``You are buying this world, `#" + world->name + "``. Don't buy a world just to get its rare items, because scammers can easily place secret doors allowing someone to jump in and `4steal the items back`` after a trade!|left|\nadd_spacer|small|\nadd_textbox|Also, all ratings will be removed from a world when it is traded. Don't buy a world for its ranking!|left|\nadd_spacer|small|\nadd_textbox|To be safe, only buy a world for its name and placed blocks, not loose items or high ratings. Consider yourself warned!|left|\nadd_spacer|small|\nadd_textbox|This world is locked by " + lockedwith + ".|left|\nadd_spacer|small|";
													warningkeypeer = "\nadd_textbox|`4WARNING: ``You are about to sell your world `#" + world->name + "`` - the world lock ownership will be transferred over to `w" + static_cast<PlayerInfo*>(currentPeer->data)->displayName + "`o.|left|\nadd_spacer|small|";
													break;
												}
											}
											for (auto& f : static_cast<PlayerInfo*>(currentPeer->data)->tradeItems) {
												if (f.id == 1424) {
													for (int i = 0; i < world->width * world->height; i++) {
														if (getItemDef(world->items.at(i).foreground).blockType == BlockTypes::LOCK) {
															if (world->items.at(i).foreground == 202 || world->items.at(i).foreground == 204 || world->items.at(i).foreground == 206 || world->items.at(i).foreground == 4994) continue;
															lockedwith = getItemDef(world->items.at(i).foreground).name;
															break;
														}
													}
													warningkeypeer = "\nadd_textbox|`4SCAM WARNING: ``You are buying this world, `#" + world->name + "``. Don't buy a world just to get its rare items, because scammers can easily place secret doors allowing someone to jump in and `4steal the items back`` after a trade!|left|\nadd_spacer|small|\nadd_textbox|Also, all ratings will be removed from a world when it is traded. Don't buy a world for its ranking!|left|\nadd_spacer|small|\nadd_textbox|To be safe, only buy a world for its name and placed blocks, not loose items or high ratings. Consider yourself warned!|left|\nadd_spacer|small|\nadd_textbox|This world is locked by " + lockedwith + ".|left|\nadd_spacer|small|";
													warningkeycurrent = "\nadd_textbox|`4WARNING: ``You are about to sell your world `#" + world->name + "`` - the world lock ownership will be transferred over to `w" + pData->displayName + "`o.|left|\nadd_spacer|small|";
													break;
												}
											}

											Player::OnDialogRequest(peer, "set_default_color|`o\nadd_label_with_icon|big|`wTrade Confirmation``|left|1366|\nadd_spacer|small|\nadd_textbox|`4You'll give:``|left|\nadd_spacer|small|" + FormatTradeItemsForDialog(pData->tradeItems, 0) + "\nadd_spacer|small|\nadd_textbox|`2You'll get:``|left|\nadd_spacer|small|" + FormatTradeItemsForDialog(static_cast<PlayerInfo*>(currentPeer->data)->tradeItems, 1) + "\nadd_spacer|small|" + warningkeypeer + warning1 + "\nadd_button|accept|Do The Trade!|noflags|0|0|\nadd_button|back|Cancel|noflags|0|0|\nend_dialog|trade_confirm|||");
											Player::OnDialogRequest(currentPeer, "set_default_color|`o\nadd_label_with_icon|big|`wTrade Confirmation``|left|1366|\nadd_spacer|small|\nadd_textbox|`4You'll give:``|left|\nadd_spacer|small|" + FormatTradeItemsForDialog(static_cast<PlayerInfo*>(currentPeer->data)->tradeItems, 0) + "\nadd_spacer|small|\nadd_textbox|`2You'll get:``|left|\nadd_spacer|small|" + FormatTradeItemsForDialog(pData->tradeItems, 1) + "\nadd_spacer|small|" + warningkeycurrent + warning2 + "\nadd_button|accept|Do The Trade!|noflags|0|0|\nadd_button|back|Cancel|noflags|0|0|\nend_dialog|trade_confirm|||");
										}
										break;
									}
								}
							}
						}
						catch (const std::out_of_range& e) {
							std::cout << e.what() << std::endl;
						}
						break;
					}
					// all store locations. red gem, options/store, ect...
					else if (cch == "action|store\nlocation|gem\n" || cch == "action|store\nlocation|bottommenu\n" || cch == "action|buy\nitem|main\n" || cch == "action|storenavigate\nitem|main\nselection|gems_rain\n")
					{
						if (!pData->haveGrowId)
						{
							SendRegisterDialog(peer);
							break;
						}
						try
						{
							string items_here = "";
							items_here += "\nadd_banner|interface/large/gui_shop_featured_header.rttex|0|1|";
							if (SurgeryDay)
							{
								items_here += "\nadd_image_button|image_button|interface/large/gui_event_banner1.rttex|bannerlayout|OPENDIALOG|surg_day_dialog|";
								items_here += "\nadd_button|surgical_kit|`oSurgical Kit``|interface/large/store_buttons/store_buttons7.rttex|`2You Get:`` 1 `#Rare Heart Monitor``, 1 Hospital Bed, 1 Train-E Bot, 5 of each of the 13 different Surical Tools and 10 Med-a-Checks.<CR><CR>`5Description:`` Get all the tools you need to become Chief of Surgery at Growtopia General Hospital! `#Rare`` Heart Monitor that lets people know when you are online, Hospital Bed that lets you perform surgery on anybody laying (or standing) on it, Med-a-Checks to identify patients with maladies, The Train-E bot to practice on, and 5 each of the thirteen different Surgical Tools you'll need to do that surgery!|0|2|12000|0|||-1|-1||-1|-1||1||||||0|0|";
							}
							items_here += "\nadd_button|iotm|`oItem Of The Month``|interface/large/store_buttons/store_buttons16.rttex|September 2021:`` `9Ouroboros Charm``!<CR><CR>`2You Get:`` 1 `9Ouroboros Charm``.<CR><CR>`5Description:`` The endless loop of life and death, personified and celebrated. is it a charm or is it a curse?|0|3|100000|0||interface/large/gui_store_button_overlays1.rttex|0|0|/interface/large/gui_shop_buybanner.rttex|0|2||1||||||0|0|";
							items_here += "\nadd_banner|interface/large/gui_shop_featured_header.rttex|0|2|";
							items_here += "\nadd_big_banner|interface/large/gui_store_iap_message.rttex|0|0|`0You want to buy gems?! Great! But you'll need to add a `2GrowID`` to this account first, to make sure you won't lose what you buy if you drop your device in the toilet. It's free!|";
							items_here += "\nadd_banner|interface/large/gui_shop_featured_header.rttex|0|3|";
							items_here += "\nadd_big_banner|interface/large/gui_store_iap_message.rttex|0|1|`0You want to buy gems?! Great! But you'll need to add a `2GrowID`` to this account first, to make sure you won't lose what you buy if you drop your device in the toilet. It's free!|";
							GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStoreRequest"), "set_description_text|Welcome to the `2Growtopia Store``! Select the item you'd like more info on.`o `wWant to get `5Supporter`` status? Any Gem purchase (or `57,000`` Gems earned with free `5Tapjoy`` offers) will make you one. You'll get new skin colors, the `5Recycle`` tool to convert unwanted items into Gems, and more bonuses!\nenable_tabs|1\nadd_tab_button|main_menu|Home|interface/large/btn_shop2.rttex||1|0|0|0||||-1|-1|||0|\nadd_tab_button|locks_menu|Locks And Stuff|interface/large/btn_shop2.rttex||0|1|0|0||||-1|-1|||0|\nadd_tab_button|itempack_menu|Item Packs|interface/large/btn_shop2.rttex||0|3|0|0||||-1|-1|||0|\nadd_tab_button|bigitems_menu|Awesome Items|interface/large/btn_shop2.rttex||0|4|0|0||||-1|-1|||0|\nadd_tab_button|weather_menu|Weather Machines|interface/large/btn_shop2.rttex|Tired of the same sunny sky?  We offer alternatives within...|0|5|0|0||||-1|-1|||0|\nadd_tab_button|token_menu|Growtoken Items|interface/large/btn_shop2.rttex||0|2|0|0||||-1|-1|||0|" + items_here + ""));
							ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
							enet_peer_send(peer, 0, packet);
							delete p.data;
						}
						catch (const std::out_of_range& e)
						{
							std::cout << e.what() << std::endl;
						}
						break;
					}
					else if (cch == "action|buy\nitem|locks\n" || cch == "action|storenavigate\nitem|locks\nselection|upgrade_backpack\n")
					{
						string UpgradeInventoryDialog = "";
						if (pData->currentInventorySize <= 190)
						{
							UpgradeInventoryDialog = "\nadd_button|upgrade_backpack|`0Upgrade Backpack`` (`w10 Slots``)|interface/large/store_buttons/store_buttons.rttex|`2You Get:`` 10 Additional Backpack Slots.<CR><CR>`5Description:`` Sewing an extra pocket onto your backpack will allow you to store `$10`` additional item types.  How else are you going to fit all those toilets and doors?|0|1|10000|0|||-1|-1||-1|-1||1||||||0|";
						}
						GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStoreRequest"), "set_description_text|`2Locks And Stuff!``  Select the item you'd like more info on, or BACK to go back.\nenable_tabs|1\nadd_tab_button|main_menu|Home|interface/large/btn_shop2.rttex||0|0|0|0||||-1|-1|||0|\nadd_tab_button|locks_menu|Locks And Stuff|interface/large/btn_shop2.rttex||1|1|0|0||||-1|-1|||0|\nadd_tab_button|itempack_menu|Item Packs|interface/large/btn_shop2.rttex||0|3|0|0||||-1|-1|||0|\nadd_tab_button|bigitems_menu|Awesome Items|interface/large/btn_shop2.rttex||0|4|0|0||||-1|-1|||0|\nadd_tab_button|weather_menu|Weather Machines|interface/large/btn_shop2.rttex|Tired of the same sunny sky?  We offer alternatives within...|0|5|0|0||||-1|-1|||0|\nadd_tab_button|token_menu|Growtoken Items|interface/large/btn_shop2.rttex||0|2|0|0||||-1|-1|||0|" + UpgradeInventoryDialog + "\nadd_button|rename|`oBirth Certificate``|interface/large/store_buttons/store_buttons7.rttex|`2You Get:`` 1 Birth Certificate.<CR><CR>`5Description:`` Tired of being who you are? By forging a new birth certificate, you can change your GrowID! The Birth Certificate will be consumed when used. This item only works if you have a GrowID, and you can only use one every 60 days, so you're not confusing everybody.|0|6|20000|0|||-1|-1||-1|-1||1||||||0|\nadd_button|clothes|`oClothes Pack``|interface/large/store_buttons/store_buttons2.rttex|`2You Get:`` 3 Randomly Wearble Items.<CR><CR>`5Description:`` Why not look the part? Some may even have special powers...|0|0|50|0|||-1|-1||-1|-1||1||||||0|\nadd_button|rare_clothes|`oRare Clothes Pack``|interface/large/store_buttons/store_buttons2.rttex|`2You Get:`` 3 Randomly Chosen Wearbale Items.<CR><CR>`5Description:`` Enjoy the garb of kings! Some may even have special powers...|0|1|500|0|||-1|-1||-1|-1||1||||||0|\nadd_button|transmutation_device|`oTransmutabooth``|interface/large/store_buttons/store_buttons27.rttex|`2You Get:`` 1 Transmutabooth.<CR><CR>`5Description:`` Behold! A wondrous technological achievement from the innovative minds at GrowTech, the Transmutabooth allows you to merge clothing items, transferring the visual appearance of one onto another in the same slot! If you've ever wanted your Cyclopean Visor to look like Shades (while keeping its mod), now you can!|0|7|25000|0|||-1|-1||-1|-1||1||||||0|\nadd_button|nyan_hat|`oTurtle Hat``|interface/large/store_buttons/store_buttons3.rttex|`2You Get:`` 1 Turtle Hat.<CR><CR>`5Description:`` It's the greatest hat ever. It bloops out bubbles as you run! `4Not available any other way!``|0|2|25000|0|||-1|-1||-1|-1||1||||||0|\nadd_button|tiny_horsie|`oTiny Horsie``|interface/large/store_buttons/store_buttons3.rttex|`2You Get:`` 1 Tiny Horsie.<CR><CR>`5Description:`` Tired of wearing shoes? Wear a Tiny Horsie instead! Or possibly a large dachshund, we're not sure. Regardless, it lets you run around faster than normal, plus you're on a horse! `4Not available any other way!``|0|5|25000|0|||-1|-1||-1|-1||1||||||0|\nadd_button|star_ship|`oPleiadian Star Ship``|interface/large/store_buttons/store_buttons4.rttex|`2You Get:`` 1 Pleiadian Star Ship.<CR><CR>`5Description:`` Float on, my brother. It's all groovy. This star ship can't fly, but you can still zoom around in it, leaving a trail of energy rings and moving at enhanced speed. Sponsored by Pleiadian. `4Not available any other way!``|0|3|25000|0|||-1|-1||-1|-1||1||||||0|\nadd_button|dragon_hand|`oDragon Hand``|interface/large/store_buttons/store_buttons5.rttex|`2You Get:`` 1 Dragon Hand.<CR><CR>`5Description:`` Call forth the dragons of legend!  With the Dragon Hand, you will command your own pet dragon. Instead of punching blocks or players, you can order your dragon to incinerate them! In addition to just being awesome, this also does increased damage, and pushes other players farther. `4Not available any other way!``|0|1|50000|0|||-1|-1||-1|-1||1||||||0|\nadd_button|corvette|`oLittle Red Corvette``|interface/large/store_buttons/store_buttons6.rttex|`2You Get:`` 1 Little Red Corvette.<CR><CR>`5Description:`` Cruise around the neighborhood in style with this sweet convertible. It moves at enhanced speed and leaves other Growtopians in your dust. `4Not available any other way!``|0|1|25000|0|||-1|-1||-1|-1||1||||||0|\nadd_button|stick_horse|`oStick Horse``|interface/large/store_buttons/store_buttons6.rttex|`2You Get:`` 1 Stick Horse.<CR><CR>`5Description:`` Nobody looks cooler than a person bouncing along on a stick with a fake horse head attached. NOBODY. `4Not available any other way!``|0|3|25000|0|||-1|-1||-1|-1||1||||||0|\nadd_button|ambulance|`oAmbulance``|interface/large/store_buttons/store_buttons7.rttex|`2You Get:`` 1 Ambulance.<CR><CR>`5Description:`` Rush to the scene of an accident while lawyers chase you in this speedy rescue vehicle. `4Not available any other way!``|0|3|25000|0|||-1|-1||-1|-1||1||||||0|\nadd_button|raptor|`oRiding Raptor``|interface/large/store_buttons/store_buttons7.rttex|`2You Get:`` 1 Riding Raptor.<CR><CR>`5Description:`` Long thought to be extinct, it turns out that these dinosaurs are actually alive and easily tamed. And riding one lets you run around faster than normal! `4Not available any other way!``|0|7|25000|0|||-1|-1||-1|-1||1||||||0|\nadd_button|owl|`oMid-Pacific Owl``|interface/large/store_buttons/store_buttons10.rttex|`2You Get:`` 1 Mid-Pacific Owl.<CR><CR>`5Description:`` This owl is a bit lazy - if you stop moving around, he'll land on your head and fall asleep. Dedicated to the students of the Mid-Pacific Institute. `4Not available any other way!``|0|1|30000|0|||-1|-1||-1|-1||1||||||0|\nadd_button|unicorn|`oUnicorn Garland``|interface/large/store_buttons/store_buttons10.rttex|`2You Get:`` 1 Unicorn Garland.<CR><CR>`5Description:`` Prance about in the fields with your very own pet unicorn! It shoots `1R`2A`3I`4N`5B`6O`7W`8S``. `4Not available any other way!``|0|4|50000|0|||-1|-1||-1|-1||1||||||0|\nadd_button|starboard|`oStarBoard``|interface/large/store_buttons/store_buttons11.rttex|`2You Get:`` 1 StarBoard.<CR><CR>`5Description:`` Hoverboards are here at last! Zoom around Growtopia on this brand new model, which is powered by fusion energy (that means stars spit out of the bottom). Moves faster than walking. Sponsored by Miwsky, Chudy, and Dawid. `4Not available any other way!``|0|1|30000|0|||-1|-1||-1|-1||1||||||0|\nadd_button|motorcycle|`oGrowley Motorcycle``|interface/large/store_buttons/store_buttons11.rttex|`2You Get:`` 1 Growley Motorcycle.<CR><CR>`5Description:`` The coolest motorcycles available are Growley Dennisons. Get a sporty blue one today! It even moves faster than walking, which is pretty good for a motorcycle. `4Not available any other way!``|0|6|50000|0|||-1|-1||-1|-1||1||||||0|\nadd_button|monkey_on_back|`oMonkey On Your Back``|interface/large/store_buttons/store_buttons13.rttex|`2You Get:`` 1 Monkey On Your Back.<CR><CR>`5Description:`` Most people work really hard to get rid of these, but hey, if you want one, it's available! `4But not available any other way!`` Sponsored by SweGamerHD's subscribers, Kizashi, and Inforced. `#Note: This is a neck item, not a back item. He's grabbing your neck!``|0|2|50000|0|||-1|-1||-1|-1||1||||||0|\nadd_button|carrot_sword|`oCarrot Sword``|interface/large/store_buttons/store_buttons13.rttex|`2You Get:`` 1 Carrot Sword.<CR><CR>`5Description:`` Razor sharp, yet oddly tasty. This can carve bunny symbols into your foes! `4Not available any other way!`` Sponsored by MrMehMeh.|0|3|15000|0|||-1|-1||-1|-1||1||||||0|\nadd_button|red_bicycle|`oRed Bicycle``|interface/large/store_buttons/store_buttons13.rttex|`2You Get:`` 1 Red Bicycle.<CR><CR>`5Description:`` It's the environmentally friendly way to get around! Ride this bicycle at high speed hither and zither throughout Growtopia. `4Not available any other way!``|0|5|30000|0|||-1|-1||-1|-1||1||||||0|\nadd_button|fire_truck|`oFire Truck``|interface/large/store_buttons/store_buttons14.rttex|`2You Get:`` 1 Fire Truck.<CR><CR>`5Description:`` Race to the scene of the fire in this speedy vehicle! `4Not available any other way!``|0|2|50000|0|||-1|-1||-1|-1||1||||||0|\nadd_button|pet_slime|`oPet Slime``|interface/large/store_buttons/store_buttons14.rttex|`2You Get:`` 1 Pet Slime.<CR><CR>`5Description:`` What could be better than a blob of greasy slime that follows you around? How about a blob of greasy slime that follows you around and spits corrosive acid, melting blocks more quickly than a normal punch? `4Not available any other way!``|0|4|100000|0|||-1|-1||-1|-1||1||||||0|\nadd_button|dabstep_shoes|`oDabstep Low Top Sneakers``|interface/large/store_buttons/store_buttons21.rttex|`2You Get:`` 1 Dabstep Low Top Sneakers.<CR><CR>`5Description:`` Light up every footfall and move to a better beat with these dabulous shoes! When you're wearing these, the world is your dance floor! `4Not available any other way!``|0|2|30000|0|||-1|-1||-1|-1||1||||||0|"));
						ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
						enet_peer_send(peer, 0, packet);
						delete p.data;
						break;
					}
					else if (cch.find("action|buy") == 0) {
						if (cch == "action|buy\nitem|iotm\n") {
							/*Fast Item Setup*/
							auto Price = 100000;
							auto ItemID = 11232;
							ifstream ifsz("save/gemdb/_" + pData->rawName + ".txt");
							string content((std::istreambuf_iterator<char>(ifsz)), (std::istreambuf_iterator<char>()));
							auto gembux = atoi(content.c_str());
							if (gembux >= Price) {
								if (CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && CheckItemExists(peer, ItemID) && CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && !CheckItemExists(peer, ItemID)) {
									Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
									GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You don't have enough space in your inventory that. You may be carrying to many of one of the items you are trying to purchase or you don't have enough free spaces to fit them all in your backpack!"));
									ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
									enet_peer_send(peer, 0, packet);
									delete p.data;
									break;
								}
								gembux -= Price;
								ofstream myfile;
								myfile.open("save/gemdb/_" + pData->rawName + ".txt");
								myfile << gembux;
								myfile.close();
								Player::OnSetBux(peer, gembux, 0);
								bool success = true;
								SaveItemMoreTimes(ItemID, 1, peer, success, "Purchased from store");
								Player::PlayAudio(peer, "audio/piano_nice.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You've purchased `o" + getItemDef(ItemID).name + " `wfor `$" + to_string(Price) + " `wGems.\nYou have `$" + to_string(gembux) + " `wGems left.\n\n`5Received: ``" + getItemDef(ItemID).name + ""));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
							else {
								Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You can't afford `o" + getItemDef(ItemID).name + "``!  You're `$" + to_string(Price - gembux) + "`` Gems short."));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
						}
						if (cch == "action|buy\nitem|rename\n") {
							/*Fast Item Setup*/
							auto Price = 20000;
							auto ItemID = 1280;
							ifstream ifsz("save/gemdb/_" + pData->rawName + ".txt");
							string content((std::istreambuf_iterator<char>(ifsz)), (std::istreambuf_iterator<char>()));
							auto gembux = atoi(content.c_str());
							if (gembux >= Price) {
								if (CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && CheckItemExists(peer, ItemID) && CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && !CheckItemExists(peer, ItemID)) {
									Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
									GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You don't have enough space in your inventory that. You may be carrying to many of one of the items you are trying to purchase or you don't have enough free spaces to fit them all in your backpack!"));
									ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
									enet_peer_send(peer, 0, packet);
									delete p.data;
									break;
								}
								gembux -= Price;
								ofstream myfile;
								myfile.open("save/gemdb/_" + pData->rawName + ".txt");
								myfile << gembux;
								myfile.close();
								Player::OnSetBux(peer, gembux, 0);
								bool success = true;
								SaveItemMoreTimes(ItemID, 1, peer, success, "Purchased from store");
								Player::PlayAudio(peer, "audio/piano_nice.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You've purchased `o" + getItemDef(ItemID).name + " `wfor `$" + to_string(Price) + " `wGems.\nYou have `$" + to_string(gembux) + " `wGems left.\n\n`5Received: ``" + getItemDef(ItemID).name + ""));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
							else {
								Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You can't afford `o" + getItemDef(ItemID).name + "``!  You're `$" + to_string(Price - gembux) + "`` Gems short."));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
						}
						if (cch == "action|buy\nitem|rare_clothes\n") {
							/*Fast Item Setup*/
							auto Price = 500;
							ifstream ifsz("save/gemdb/_" + pData->rawName + ".txt");
							string content((std::istreambuf_iterator<char>(ifsz)), (std::istreambuf_iterator<char>()));
							auto gembux = atoi(content.c_str());
							if (gembux >= Price) {
								vector<int> rand_items{ 250, 1716, 82, 78, 94, 662, 754, 146, 356, 72, 46, 124, 1720, 92, 1122, 212, 84, 1722, 1844, 2718, 70, 4966, 288, 232, 788, 148, 96, 3932, 448, 98, 134, 50, 74, 1504, 494, 266, 852, 132, 2954, 3934, 3060, 3056, 1800, 88, 2960, 1022, 2578, 3164, 86, 302, 1332, 304, 1116, 6788, 894, 492, 1690, 316, 1850, 1718, 1838, 1032, 1860, 1026, 3048, 352, 724, 1590, 798, 488, 1342, 794, 268, 1502, 8884, 1858, 594, 3466, 1314, 1688, 1312, 3590, 446, 1856, 1854, 1516, 1316, 2700, 4968, 4964, 6796, 1862, 310, 290, 296, 6674, 1864, 4954, 1476, 896, 1848, 496, 136, 158, 2958, 2576, 1954, 152, 1126, 796, 1524, 2928, 80, 2934, 150, 1120, 1142, 1478, 154, 9356, 452, 258, 2882, 156, 1140, 3530, 312, 1124, 890, 1330, 3052, 294, 1650, 1652, 348, 1736, 576, 5016, 1708, 292, 1146, 1028, 1762, 600 };
								int ItemID = rand_items[rand() % rand_items.size()];
								int icount = 1;
								int ItemID2 = rand_items[rand() % rand_items.size()];
								int icount2 = 1;
								int ItemID3 = rand_items[rand() % rand_items.size()];
								int icount3 = 1;
								if (ItemID == ItemID2) {
									ItemID2 = 0;
									icount++;
								}
								if (ItemID == ItemID3) {
									ItemID3 = 0;
									icount++;
								}
								if (ItemID2 == ItemID3) {
									ItemID3 = 0;
									icount2++;
								}
								if (CheckItemMaxed(peer, ItemID, 1) || CheckItemMaxed(peer, ItemID2, 1) || CheckItemMaxed(peer, ItemID3, 1) || pData->inventory.items.size() + 3 >= pData->currentInventorySize) {
									Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
									GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You don't have enough space in your inventory that. You may be carrying to many of one of the items you are trying to purchase or you don't have enough free spaces to fit them all in your backpack!"));
									ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
									enet_peer_send(peer, 0, packet);
									delete p.data;
									break;
								}
								gembux -= Price;
								ofstream myfile;
								myfile.open("save/gemdb/_" + pData->rawName + ".txt");
								myfile << gembux;
								myfile.close();
								Player::OnSetBux(peer, gembux, 0);
								bool success = true;
								SaveItemMoreTimes(ItemID, icount, peer, success, "Purchased from store");
								if (ItemID2 != 0) SaveItemMoreTimes(ItemID2, icount2, peer, success, "Purchased from store");
								if (ItemID3 != 0) SaveItemMoreTimes(ItemID3, icount3, peer, success, "Purchased from store");
								Player::PlayAudio(peer, "audio/piano_nice.wav", 0);
								string result = "`5Received: ``";
								if (icount == 1) {
									result += getItemDef(ItemID).name;
								}
								else {
									result += to_string(icount) + " " + getItemDef(ItemID).name;
								}
								if (ItemID2 != 0) {
									result += "`$,`` ";
									if (icount2 == 1) {
										result += getItemDef(ItemID2).name;
									}
									else {
										result += to_string(icount2) + " " + getItemDef(ItemID2).name;
									}
								}
								if (ItemID3 != 0) {
									result += "`$,`` ";
									if (icount3 == 1) {
										result += getItemDef(ItemID3).name;
									}
									else {
										result += to_string(icount3) + " " + getItemDef(ItemID3).name;
									}
								}
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You've purchased `oRare Clothes Pack`` `wfor `$" + to_string(Price) + " `wGems.\nYou have `$" + to_string(gembux) + " `wGems left.\n\n" + result));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
							else {
								Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You can't afford `oRare Clothes Pack``!  You're `$" + to_string(Price - gembux) + "`` Gems short."));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
						}
						if (cch == "action|buy\nitem|clothes\n") {
							/*Fast Item Setup*/
							auto Price = 50;
							ifstream ifsz("save/gemdb/_" + pData->rawName + ".txt");
							string content((std::istreambuf_iterator<char>(ifsz)), (std::istreambuf_iterator<char>()));
							auto gembux = atoi(content.c_str());
							if (gembux >= Price) {
								vector<int> rand_items{ 38, 34, 210, 66, 370, 234, 238, 1846, 144 };
								int ItemID = rand_items[rand() % rand_items.size()];
								int icount = 1;
								int ItemID2 = rand_items[rand() % rand_items.size()];
								int icount2 = 1;
								int ItemID3 = rand_items[rand() % rand_items.size()];
								int icount3 = 1;
								if (ItemID == ItemID2) {
									ItemID2 = 0;
									icount++;
								}
								if (ItemID == ItemID3) {
									ItemID3 = 0;
									icount++;
								}
								if (ItemID2 == ItemID3) {
									ItemID3 = 0;
									icount2++;
								}
								if (CheckItemMaxed(peer, ItemID, 1) || CheckItemMaxed(peer, ItemID2, 1) || CheckItemMaxed(peer, ItemID3, 1) || pData->inventory.items.size() + 3 >= pData->currentInventorySize) {
									Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
									GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You don't have enough space in your inventory that. You may be carrying to many of one of the items you are trying to purchase or you don't have enough free spaces to fit them all in your backpack!"));
									ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
									enet_peer_send(peer, 0, packet);
									delete p.data;
									break;
								}
								gembux -= Price;
								ofstream myfile;
								myfile.open("save/gemdb/_" + pData->rawName + ".txt");
								myfile << gembux;
								myfile.close();
								Player::OnSetBux(peer, gembux, 0);
								bool success = true;
								SaveItemMoreTimes(ItemID, icount, peer, success, "Purchased from store");
								if (ItemID2 != 0) SaveItemMoreTimes(ItemID2, icount2, peer, success, "Purchased from store");
								if (ItemID3 != 0) SaveItemMoreTimes(ItemID3, icount3, peer, success, "Purchased from store");
								Player::PlayAudio(peer, "audio/piano_nice.wav", 0);
								string result = "`5Received: ``";
								if (icount == 1) {
									result += getItemDef(ItemID).name;
								}
								else {
									result += to_string(icount) + " " + getItemDef(ItemID).name;
								}
								if (ItemID2 != 0) {
									result += "`$,`` ";
									if (icount2 == 1) {
										result += getItemDef(ItemID2).name;
									}
									else {
										result += to_string(icount2) + " " + getItemDef(ItemID2).name;
									}
								}
								if (ItemID3 != 0) {
									result += "`$,`` ";
									if (icount3 == 1) {
										result += getItemDef(ItemID3).name;
									}
									else {
										result += to_string(icount3) + " " + getItemDef(ItemID3).name;
									}
								}
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You've purchased `oClothes Pack`` `wfor `$" + to_string(Price) + " `wGems.\nYou have `$" + to_string(gembux) + " `wGems left.\n\n" + result));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
							else {
								Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You can't afford `oClothes Pack``!  You're `$" + to_string(Price - gembux) + "`` Gems short."));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
						}
						if (cch == "action|buy\nitem|address_change\n") {
							/*Fast Item Setup*/
							auto Price = 20000;
							auto ItemID = 2580;
							ifstream ifsz("save/gemdb/_" + pData->rawName + ".txt");
							string content((std::istreambuf_iterator<char>(ifsz)), (std::istreambuf_iterator<char>()));
							auto gembux = atoi(content.c_str());
							if (gembux >= Price) {
								if (CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && CheckItemExists(peer, ItemID) && CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && !CheckItemExists(peer, ItemID)) {
									Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
									GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You don't have enough space in your inventory that. You may be carrying to many of one of the items you are trying to purchase or you don't have enough free spaces to fit them all in your backpack!"));
									ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
									enet_peer_send(peer, 0, packet);
									delete p.data;
									break;
								}
								gembux -= Price;
								ofstream myfile;
								myfile.open("save/gemdb/_" + pData->rawName + ".txt");
								myfile << gembux;
								myfile.close();
								Player::OnSetBux(peer, gembux, 0);
								bool success = true;
								SaveItemMoreTimes(ItemID, 1, peer, success, "Purchased from store");
								Player::PlayAudio(peer, "audio/piano_nice.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You've purchased `o" + getItemDef(ItemID).name + " `wfor `$" + to_string(Price) + " `wGems.\nYou have `$" + to_string(gembux) + " `wGems left.\n\n`5Received: ``" + getItemDef(ItemID).name + ""));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
							else {
								Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You can't afford `o" + getItemDef(ItemID).name + "``!  You're `$" + to_string(Price - gembux) + "`` Gems short."));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
						}
						if (cch == "action|buy\nitem|vending_machine\n") {
							/*Fast Item Setup*/
							auto Price = 8000;
							auto ItemID = 2978;
							ifstream ifsz("save/gemdb/_" + pData->rawName + ".txt");
							string content((std::istreambuf_iterator<char>(ifsz)), (std::istreambuf_iterator<char>()));
							auto gembux = atoi(content.c_str());
							if (gembux >= Price) {
								if (CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && CheckItemExists(peer, ItemID) && CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && !CheckItemExists(peer, ItemID)) {
									Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
									GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You don't have enough space in your inventory that. You may be carrying to many of one of the items you are trying to purchase or you don't have enough free spaces to fit them all in your backpack!"));
									ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
									enet_peer_send(peer, 0, packet);
									delete p.data;
									break;
								}
								gembux -= Price;
								ofstream myfile;
								myfile.open("save/gemdb/_" + pData->rawName + ".txt");
								myfile << gembux;
								myfile.close();
								Player::OnSetBux(peer, gembux, 0);
								bool success = true;
								SaveItemMoreTimes(ItemID, 1, peer, success, "Purchased from store");
								Player::PlayAudio(peer, "audio/piano_nice.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You've purchased `o" + getItemDef(ItemID).name + " `wfor `$" + to_string(Price) + " `wGems.\nYou have `$" + to_string(gembux) + " `wGems left.\n\n`5Received: ``" + getItemDef(ItemID).name + ""));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
							else {
								Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You can't afford `o" + getItemDef(ItemID).name + "``!  You're `$" + to_string(Price - gembux) + "`` Gems short."));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
						}
						if (cch == "action|buy\nitem|grow_spray\n") {
							/*Fast Item Setup*/
							auto Price = 400;
							auto ItemID = 228;
							ifstream ifsz("save/gemdb/_" + pData->rawName + ".txt");
							string content((std::istreambuf_iterator<char>(ifsz)), (std::istreambuf_iterator<char>()));
							auto gembux = atoi(content.c_str());
							if (gembux >= Price) {
								if (CheckItemMaxed(peer, ItemID, 5) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && CheckItemExists(peer, ItemID) && CheckItemMaxed(peer, ItemID, 5) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && !CheckItemExists(peer, ItemID)) {
									Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
									GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You don't have enough space in your inventory that. You may be carrying to many of one of the items you are trying to purchase or you don't have enough free spaces to fit them all in your backpack!"));
									ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
									enet_peer_send(peer, 0, packet);
									delete p.data;
									break;
								}
								gembux -= Price;
								ofstream myfile;
								myfile.open("save/gemdb/_" + pData->rawName + ".txt");
								myfile << gembux;
								myfile.close();
								Player::OnSetBux(peer, gembux, 0);
								bool success = true;
								SaveItemMoreTimes(ItemID, 5, peer, success, "Purchased from store");
								Player::PlayAudio(peer, "audio/piano_nice.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You've purchased `o5 " + getItemDef(ItemID).name + " `wfor `$" + to_string(Price) + " `wGems.\nYou have `$" + to_string(gembux) + " `wGems left.\n\n`5Received:`` 5 " + getItemDef(ItemID).name + ""));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
							else {
								Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You can't afford `o5 " + getItemDef(ItemID).name + "``!  You're `$" + to_string(Price - gembux) + "`` Gems short."));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
						}
						if (cch == "action|buy\nitem|deluxe_grow_spray\n") {
							/*Fast Item Setup*/
							auto Price = 2000;
							auto ItemID = 1778;
							ifstream ifsz("save/gemdb/_" + pData->rawName + ".txt");
							string content((std::istreambuf_iterator<char>(ifsz)), (std::istreambuf_iterator<char>()));
							auto gembux = atoi(content.c_str());
							if (gembux >= Price) {
								if (CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && CheckItemExists(peer, ItemID) && CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && !CheckItemExists(peer, ItemID)) {
									Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
									GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You don't have enough space in your inventory that. You may be carrying to many of one of the items you are trying to purchase or you don't have enough free spaces to fit them all in your backpack!"));
									ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
									enet_peer_send(peer, 0, packet);
									delete p.data;
									break;
								}
								gembux -= Price;
								ofstream myfile;
								myfile.open("save/gemdb/_" + pData->rawName + ".txt");
								myfile << gembux;
								myfile.close();
								Player::OnSetBux(peer, gembux, 0);
								bool success = true;
								SaveItemMoreTimes(ItemID, 1, peer, success, "Purchased from store");
								Player::PlayAudio(peer, "audio/piano_nice.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You've purchased `o" + getItemDef(ItemID).name + " `wfor `$" + to_string(Price) + " `wGems.\nYou have `$" + to_string(gembux) + " `wGems left.\n\n`5Received: ``" + getItemDef(ItemID).name + ""));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
							else {
								Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You can't afford `o" + getItemDef(ItemID).name + "``!  You're `$" + to_string(Price - gembux) + "`` Gems short."));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
						}
						if (cch == "action|buy\nitem|antigravity_generator\n") {
							/*Fast Item Setup*/
							auto Price = 450000;
							auto ItemID = 4992;
							ifstream ifsz("save/gemdb/_" + pData->rawName + ".txt");
							string content((std::istreambuf_iterator<char>(ifsz)), (std::istreambuf_iterator<char>()));
							auto gembux = atoi(content.c_str());
							if (gembux >= Price) {
								if (CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && CheckItemExists(peer, ItemID) && CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && !CheckItemExists(peer, ItemID)) {
									Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
									GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You don't have enough space in your inventory that. You may be carrying to many of one of the items you are trying to purchase or you don't have enough free spaces to fit them all in your backpack!"));
									ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
									enet_peer_send(peer, 0, packet);
									delete p.data;
									break;
								}
								gembux -= Price;
								ofstream myfile;
								myfile.open("save/gemdb/_" + pData->rawName + ".txt");
								myfile << gembux;
								myfile.close();
								Player::OnSetBux(peer, gembux, 0);
								bool success = true;
								SaveItemMoreTimes(ItemID, 1, peer, success, "Purchased from store");
								Player::PlayAudio(peer, "audio/piano_nice.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You've purchased `o" + getItemDef(ItemID).name + " `wfor `$" + to_string(Price) + " `wGems.\nYou have `$" + to_string(gembux) + " `wGems left.\n\n`5Received: ``" + getItemDef(ItemID).name + ""));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
							else {
								Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You can't afford `o" + getItemDef(ItemID).name + "``!  You're `$" + to_string(Price - gembux) + "`` Gems short."));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
						}
						if (cch == "action|buy\nitem|zombie_jammer\n") {
							/*Fast Item Setup*/
							auto Price = 15000;
							auto ItemID = 1278;
							ifstream ifsz("save/gemdb/_" + pData->rawName + ".txt");
							string content((std::istreambuf_iterator<char>(ifsz)), (std::istreambuf_iterator<char>()));
							auto gembux = atoi(content.c_str());
							if (gembux >= Price) {
								if (CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && CheckItemExists(peer, ItemID) && CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && !CheckItemExists(peer, ItemID)) {
									Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
									GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You don't have enough space in your inventory that. You may be carrying to many of one of the items you are trying to purchase or you don't have enough free spaces to fit them all in your backpack!"));
									ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
									enet_peer_send(peer, 0, packet);
									delete p.data;
									break;
								}
								gembux -= Price;
								ofstream myfile;
								myfile.open("save/gemdb/_" + pData->rawName + ".txt");
								myfile << gembux;
								myfile.close();
								Player::OnSetBux(peer, gembux, 0);
								bool success = true;
								SaveItemMoreTimes(ItemID, 1, peer, success, "Purchased from store");
								Player::PlayAudio(peer, "audio/piano_nice.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You've purchased `o" + getItemDef(ItemID).name + " `wfor `$" + to_string(Price) + " `wGems.\nYou have `$" + to_string(gembux) + " `wGems left.\n\n`5Received: ``" + getItemDef(ItemID).name + ""));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
							else {
								Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You can't afford `o" + getItemDef(ItemID).name + "``!  You're `$" + to_string(Price - gembux) + "`` Gems short."));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
						}
						if (cch == "action|buy\nitem|punch_jammer\n") {
							/*Fast Item Setup*/
							auto Price = 15000;
							auto ItemID = 1276;
							ifstream ifsz("save/gemdb/_" + pData->rawName + ".txt");
							string content((std::istreambuf_iterator<char>(ifsz)), (std::istreambuf_iterator<char>()));
							auto gembux = atoi(content.c_str());
							if (gembux >= Price) {
								if (CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && CheckItemExists(peer, ItemID) && CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && !CheckItemExists(peer, ItemID)) {
									Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
									GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You don't have enough space in your inventory that. You may be carrying to many of one of the items you are trying to purchase or you don't have enough free spaces to fit them all in your backpack!"));
									ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
									enet_peer_send(peer, 0, packet);
									delete p.data;
									break;
								}
								gembux -= Price;
								ofstream myfile;
								myfile.open("save/gemdb/_" + pData->rawName + ".txt");
								myfile << gembux;
								myfile.close();
								Player::OnSetBux(peer, gembux, 0);
								bool success = true;
								SaveItemMoreTimes(ItemID, 1, peer, success, "Purchased from store");
								Player::PlayAudio(peer, "audio/piano_nice.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You've purchased `o" + getItemDef(ItemID).name + " `wfor `$" + to_string(Price) + " `wGems.\nYou have `$" + to_string(gembux) + " `wGems left.\n\n`5Received: ``" + getItemDef(ItemID).name + ""));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
							else {
								Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You can't afford `o" + getItemDef(ItemID).name + "``!  You're `$" + to_string(Price - gembux) + "`` Gems short."));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
						}
						if (cch == "action|buy\nitem|signal_jammer\n") {
							/*Fast Item Setup*/
							auto Price = 2000;
							auto ItemID = 226;
							ifstream ifsz("save/gemdb/_" + pData->rawName + ".txt");
							string content((std::istreambuf_iterator<char>(ifsz)), (std::istreambuf_iterator<char>()));
							auto gembux = atoi(content.c_str());
							if (gembux >= Price) {
								if (CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && CheckItemExists(peer, ItemID) && CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && !CheckItemExists(peer, ItemID)) {
									Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
									GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You don't have enough space in your inventory that. You may be carrying to many of one of the items you are trying to purchase or you don't have enough free spaces to fit them all in your backpack!"));
									ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
									enet_peer_send(peer, 0, packet);
									delete p.data;
									break;
								}
								gembux -= Price;
								ofstream myfile;
								myfile.open("save/gemdb/_" + pData->rawName + ".txt");
								myfile << gembux;
								myfile.close();
								Player::OnSetBux(peer, gembux, 0);
								bool success = true;
								SaveItemMoreTimes(ItemID, 1, peer, success, "Purchased from store");
								Player::PlayAudio(peer, "audio/piano_nice.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You've purchased `o" + getItemDef(ItemID).name + " `wfor `$" + to_string(Price) + " `wGems.\nYou have `$" + to_string(gembux) + " `wGems left.\n\n`5Received: ``" + getItemDef(ItemID).name + ""));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
							else {
								Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You can't afford `o" + getItemDef(ItemID).name + "``!  You're `$" + to_string(Price - gembux) + "`` Gems short."));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
						}
						if (cch == "action|buy\nitem|infinity_weather_machine\n") {
							/*Fast Item Setup*/
							auto Price = 50000;
							auto ItemID = 10058;
							ifstream ifsz("save/gemdb/_" + pData->rawName + ".txt");
							string content((std::istreambuf_iterator<char>(ifsz)), (std::istreambuf_iterator<char>()));
							auto gembux = atoi(content.c_str());
							if (gembux >= Price) {
								if (CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && CheckItemExists(peer, ItemID) && CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && !CheckItemExists(peer, ItemID)) {
									Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
									GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You don't have enough space in your inventory that. You may be carrying to many of one of the items you are trying to purchase or you don't have enough free spaces to fit them all in your backpack!"));
									ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
									enet_peer_send(peer, 0, packet);
									delete p.data;
									break;
								}
								gembux -= Price;
								ofstream myfile;
								myfile.open("save/gemdb/_" + pData->rawName + ".txt");
								myfile << gembux;
								myfile.close();
								Player::OnSetBux(peer, gembux, 0);
								bool success = true;
								SaveItemMoreTimes(ItemID, 1, peer, success, "Purchased from store");
								Player::PlayAudio(peer, "audio/piano_nice.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You've purchased `o" + getItemDef(ItemID).name + " `wfor `$" + to_string(Price) + " `wGems.\nYou have `$" + to_string(gembux) + " `wGems left.\n\n`5Received: ``" + getItemDef(ItemID).name + ""));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
							else {
								Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You can't afford `o" + getItemDef(ItemID).name + "``!  You're `$" + to_string(Price - gembux) + "`` Gems short."));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
						}
						if (cch == "action|buy\nitem|treasure_blast\n") {
							/*Fast Item Setup*/
							auto Price = 10000;
							auto ItemID = 7588;
							ifstream ifsz("save/gemdb/_" + pData->rawName + ".txt");
							string content((std::istreambuf_iterator<char>(ifsz)), (std::istreambuf_iterator<char>()));
							auto gembux = atoi(content.c_str());
							if (gembux >= Price) {
								if (CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && CheckItemExists(peer, ItemID) && CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && !CheckItemExists(peer, ItemID)) {
									Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
									GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You don't have enough space in your inventory that. You may be carrying to many of one of the items you are trying to purchase or you don't have enough free spaces to fit them all in your backpack!"));
									ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
									enet_peer_send(peer, 0, packet);
									delete p.data;
									break;
								}
								gembux -= Price;
								ofstream myfile;
								myfile.open("save/gemdb/_" + pData->rawName + ".txt");
								myfile << gembux;
								myfile.close();
								Player::OnSetBux(peer, gembux, 0);
								bool success = true;
								SaveItemMoreTimes(ItemID, 1, peer, success, "Purchased from store");
								Player::PlayAudio(peer, "audio/piano_nice.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You've purchased `o" + getItemDef(ItemID).name + " `wfor `$" + to_string(Price) + " `wGems.\nYou have `$" + to_string(gembux) + " `wGems left.\n\n`5Received: ``" + getItemDef(ItemID).name + ""));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
							else {
								Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You can't afford `o" + getItemDef(ItemID).name + "``!  You're `$" + to_string(Price - gembux) + "`` Gems short."));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
						}
						if (cch == "action|buy\nitem|digital_rain_weather\n") {
							/*Fast Item Setup*/
							auto Price = 30000;
							auto ItemID = 6854;
							ifstream ifsz("save/gemdb/_" + pData->rawName + ".txt");
							string content((std::istreambuf_iterator<char>(ifsz)), (std::istreambuf_iterator<char>()));
							auto gembux = atoi(content.c_str());
							if (gembux >= Price) {
								if (CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && CheckItemExists(peer, ItemID) && CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && !CheckItemExists(peer, ItemID)) {
									Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
									GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You don't have enough space in your inventory that. You may be carrying to many of one of the items you are trying to purchase or you don't have enough free spaces to fit them all in your backpack!"));
									ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
									enet_peer_send(peer, 0, packet);
									delete p.data;
									break;
								}
								gembux -= Price;
								ofstream myfile;
								myfile.open("save/gemdb/_" + pData->rawName + ".txt");
								myfile << gembux;
								myfile.close();
								Player::OnSetBux(peer, gembux, 0);
								bool success = true;
								SaveItemMoreTimes(ItemID, 1, peer, success, "Purchased from store");
								Player::PlayAudio(peer, "audio/piano_nice.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You've purchased `o" + getItemDef(ItemID).name + " `wfor `$" + to_string(Price) + " `wGems.\nYou have `$" + to_string(gembux) + " `wGems left.\n\n`5Received: ``" + getItemDef(ItemID).name + ""));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
							else {
								Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You can't afford `o" + getItemDef(ItemID).name + "``!  You're `$" + to_string(Price - gembux) + "`` Gems short."));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
						}
						if (cch == "action|buy\nitem|weather_backgd\n") {
							/*Fast Item Setup*/
							auto Price = 150000;
							auto ItemID = 5000;
							ifstream ifsz("save/gemdb/_" + pData->rawName + ".txt");
							string content((std::istreambuf_iterator<char>(ifsz)), (std::istreambuf_iterator<char>()));
							auto gembux = atoi(content.c_str());
							if (gembux >= Price) {
								if (CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && CheckItemExists(peer, ItemID) && CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && !CheckItemExists(peer, ItemID)) {
									Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
									GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You don't have enough space in your inventory that. You may be carrying to many of one of the items you are trying to purchase or you don't have enough free spaces to fit them all in your backpack!"));
									ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
									enet_peer_send(peer, 0, packet);
									delete p.data;
									break;
								}
								gembux -= Price;
								ofstream myfile;
								myfile.open("save/gemdb/_" + pData->rawName + ".txt");
								myfile << gembux;
								myfile.close();
								Player::OnSetBux(peer, gembux, 0);
								bool success = true;
								SaveItemMoreTimes(ItemID, 1, peer, success, "Purchased from store");
								Player::PlayAudio(peer, "audio/piano_nice.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You've purchased `o" + getItemDef(ItemID).name + " `wfor `$" + to_string(Price) + " `wGems.\nYou have `$" + to_string(gembux) + " `wGems left.\n\n`5Received: ``" + getItemDef(ItemID).name + ""));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
							else {
								Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You can't afford `o" + getItemDef(ItemID).name + "``!  You're `$" + to_string(Price - gembux) + "`` Gems short."));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
						}
						if (cch == "action|buy\nitem|weather_jungle\n") {
							/*Fast Item Setup*/
							auto Price = 20000;
							auto ItemID = 4776;
							ifstream ifsz("save/gemdb/_" + pData->rawName + ".txt");
							string content((std::istreambuf_iterator<char>(ifsz)), (std::istreambuf_iterator<char>()));
							auto gembux = atoi(content.c_str());
							if (gembux >= Price) {
								if (CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && CheckItemExists(peer, ItemID) && CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && !CheckItemExists(peer, ItemID)) {
									Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
									GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You don't have enough space in your inventory that. You may be carrying to many of one of the items you are trying to purchase or you don't have enough free spaces to fit them all in your backpack!"));
									ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
									enet_peer_send(peer, 0, packet);
									delete p.data;
									break;
								}
								gembux -= Price;
								ofstream myfile;
								myfile.open("save/gemdb/_" + pData->rawName + ".txt");
								myfile << gembux;
								myfile.close();
								Player::OnSetBux(peer, gembux, 0);
								bool success = true;
								SaveItemMoreTimes(ItemID, 1, peer, success, "Purchased from store");
								Player::PlayAudio(peer, "audio/piano_nice.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You've purchased `o" + getItemDef(ItemID).name + " `wfor `$" + to_string(Price) + " `wGems.\nYou have `$" + to_string(gembux) + " `wGems left.\n\n`5Received: ``" + getItemDef(ItemID).name + ""));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
							else {
								Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You can't afford `o" + getItemDef(ItemID).name + "``!  You're `$" + to_string(Price - gembux) + "`` Gems short."));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
						}
						if (cch == "action|buy\nitem|weather_stuff\n") {
							/*Fast Item Setup*/
							auto Price = 50000;
							auto ItemID = 3832;
							ifstream ifsz("save/gemdb/_" + pData->rawName + ".txt");
							string content((std::istreambuf_iterator<char>(ifsz)), (std::istreambuf_iterator<char>()));
							auto gembux = atoi(content.c_str());
							if (gembux >= Price) {
								if (CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && CheckItemExists(peer, ItemID) && CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && !CheckItemExists(peer, ItemID)) {
									Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
									GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You don't have enough space in your inventory that. You may be carrying to many of one of the items you are trying to purchase or you don't have enough free spaces to fit them all in your backpack!"));
									ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
									enet_peer_send(peer, 0, packet);
									delete p.data;
									break;
								}
								gembux -= Price;
								ofstream myfile;
								myfile.open("save/gemdb/_" + pData->rawName + ".txt");
								myfile << gembux;
								myfile.close();
								Player::OnSetBux(peer, gembux, 0);
								bool success = true;
								SaveItemMoreTimes(ItemID, 1, peer, success, "Purchased from store");
								Player::PlayAudio(peer, "audio/piano_nice.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You've purchased `o" + getItemDef(ItemID).name + " `wfor `$" + to_string(Price) + " `wGems.\nYou have `$" + to_string(gembux) + " `wGems left.\n\n`5Received: ``" + getItemDef(ItemID).name + ""));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
							else {
								Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You can't afford `o" + getItemDef(ItemID).name + "``!  You're `$" + to_string(Price - gembux) + "`` Gems short."));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
						}
						if (cch == "action|buy\nitem|cave_blast\n") {
							/*Fast Item Setup*/
							auto Price = 750000;
							auto ItemID = 3562;
							ifstream ifsz("save/gemdb/_" + pData->rawName + ".txt");
							string content((std::istreambuf_iterator<char>(ifsz)), (std::istreambuf_iterator<char>()));
							auto gembux = atoi(content.c_str());
							if (gembux >= Price) {
								if (CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && CheckItemExists(peer, ItemID) && CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && !CheckItemExists(peer, ItemID)) {
									Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
									GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You don't have enough space in your inventory that. You may be carrying to many of one of the items you are trying to purchase or you don't have enough free spaces to fit them all in your backpack!"));
									ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
									enet_peer_send(peer, 0, packet);
									delete p.data;
									break;
								}
								gembux -= Price;
								ofstream myfile;
								myfile.open("save/gemdb/_" + pData->rawName + ".txt");
								myfile << gembux;
								myfile.close();
								Player::OnSetBux(peer, gembux, 0);
								bool success = true;
								SaveItemMoreTimes(ItemID, 1, peer, success, "Purchased from store");
								Player::PlayAudio(peer, "audio/piano_nice.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You've purchased `o" + getItemDef(ItemID).name + " `wfor `$" + to_string(Price) + " `wGems.\nYou have `$" + to_string(gembux) + " `wGems left.\n\n`5Received: ``" + getItemDef(ItemID).name + ""));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
							else {
								Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You can't afford `o" + getItemDef(ItemID).name + "``!  You're `$" + to_string(Price - gembux) + "`` Gems short."));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
						}
						if (cch == "action|buy\nitem|undersea_blast\n") {
							/*Fast Item Setup*/
							auto Price = 15000;
							auto ItemID = 1532;
							ifstream ifsz("save/gemdb/_" + pData->rawName + ".txt");
							string content((std::istreambuf_iterator<char>(ifsz)), (std::istreambuf_iterator<char>()));
							auto gembux = atoi(content.c_str());
							if (gembux >= Price) {
								if (CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && CheckItemExists(peer, ItemID) && CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && !CheckItemExists(peer, ItemID)) {
									Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
									GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You don't have enough space in your inventory that. You may be carrying to many of one of the items you are trying to purchase or you don't have enough free spaces to fit them all in your backpack!"));
									ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
									enet_peer_send(peer, 0, packet);
									delete p.data;
									break;
								}
								gembux -= Price;
								ofstream myfile;
								myfile.open("save/gemdb/_" + pData->rawName + ".txt");
								myfile << gembux;
								myfile.close();
								Player::OnSetBux(peer, gembux, 0);
								bool success = true;
								SaveItemMoreTimes(ItemID, 1, peer, success, "Purchased from store");
								Player::PlayAudio(peer, "audio/piano_nice.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You've purchased `o" + getItemDef(ItemID).name + " `wfor `$" + to_string(Price) + " `wGems.\nYou have `$" + to_string(gembux) + " `wGems left.\n\n`5Received: ``" + getItemDef(ItemID).name + ""));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
							else {
								Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You can't afford `o" + getItemDef(ItemID).name + "``!  You're `$" + to_string(Price - gembux) + "`` Gems short."));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
						}
						if (cch == "action|buy\nitem|mars_blast\n") {
							/*Fast Item Setup*/
							auto Price = 15000;
							auto ItemID = 1136;
							ifstream ifsz("save/gemdb/_" + pData->rawName + ".txt");
							string content((std::istreambuf_iterator<char>(ifsz)), (std::istreambuf_iterator<char>()));
							auto gembux = atoi(content.c_str());
							if (gembux >= Price) {
								if (CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && CheckItemExists(peer, ItemID) && CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && !CheckItemExists(peer, ItemID)) {
									Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
									GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You don't have enough space in your inventory that. You may be carrying to many of one of the items you are trying to purchase or you don't have enough free spaces to fit them all in your backpack!"));
									ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
									enet_peer_send(peer, 0, packet);
									delete p.data;
									break;
								}
								gembux -= Price;
								ofstream myfile;
								myfile.open("save/gemdb/_" + pData->rawName + ".txt");
								myfile << gembux;
								myfile.close();
								Player::OnSetBux(peer, gembux, 0);
								bool success = true;
								SaveItemMoreTimes(ItemID, 1, peer, success, "Purchased from store");
								Player::PlayAudio(peer, "audio/piano_nice.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You've purchased `o" + getItemDef(ItemID).name + " `wfor `$" + to_string(Price) + " `wGems.\nYou have `$" + to_string(gembux) + " `wGems left.\n\n`5Received: ``" + getItemDef(ItemID).name + ""));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
							else {
								Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You can't afford `o" + getItemDef(ItemID).name + "``!  You're `$" + to_string(Price - gembux) + "`` Gems short."));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
						}
						if (cch == "action|buy\nitem|weather_warp\n") {
							/*Fast Item Setup*/
							auto Price = 10000;
							auto ItemID = 1750;
							ifstream ifsz("save/gemdb/_" + pData->rawName + ".txt");
							string content((std::istreambuf_iterator<char>(ifsz)), (std::istreambuf_iterator<char>()));
							auto gembux = atoi(content.c_str());
							if (gembux >= Price) {
								if (CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && CheckItemExists(peer, ItemID) && CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && !CheckItemExists(peer, ItemID)) {
									Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
									GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You don't have enough space in your inventory that. You may be carrying to many of one of the items you are trying to purchase or you don't have enough free spaces to fit them all in your backpack!"));
									ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
									enet_peer_send(peer, 0, packet);
									delete p.data;
									break;
								}
								gembux -= Price;
								ofstream myfile;
								myfile.open("save/gemdb/_" + pData->rawName + ".txt");
								myfile << gembux;
								myfile.close();
								Player::OnSetBux(peer, gembux, 0);
								bool success = true;
								SaveItemMoreTimes(ItemID, 1, peer, success, "Purchased from store");
								Player::PlayAudio(peer, "audio/piano_nice.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You've purchased `o" + getItemDef(ItemID).name + " `wfor `$" + to_string(Price) + " `wGems.\nYou have `$" + to_string(gembux) + " `wGems left.\n\n`5Received: ``" + getItemDef(ItemID).name + ""));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
							else {
								Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You can't afford `o" + getItemDef(ItemID).name + "``!  You're `$" + to_string(Price - gembux) + "`` Gems short."));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
						}
						if (cch == "action|buy\nitem|weather_rainy\n") {
							/*Fast Item Setup*/
							auto Price = 10000;
							auto ItemID = 984;
							ifstream ifsz("save/gemdb/_" + pData->rawName + ".txt");
							string content((std::istreambuf_iterator<char>(ifsz)), (std::istreambuf_iterator<char>()));
							auto gembux = atoi(content.c_str());
							if (gembux >= Price) {
								if (CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && CheckItemExists(peer, ItemID) && CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && !CheckItemExists(peer, ItemID)) {
									Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
									GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You don't have enough space in your inventory that. You may be carrying to many of one of the items you are trying to purchase or you don't have enough free spaces to fit them all in your backpack!"));
									ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
									enet_peer_send(peer, 0, packet);
									delete p.data;
									break;
								}
								gembux -= Price;
								ofstream myfile;
								myfile.open("save/gemdb/_" + pData->rawName + ".txt");
								myfile << gembux;
								myfile.close();
								Player::OnSetBux(peer, gembux, 0);
								bool success = true;
								SaveItemMoreTimes(ItemID, 1, peer, success, "Purchased from store");
								Player::PlayAudio(peer, "audio/piano_nice.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You've purchased `o" + getItemDef(ItemID).name + " `wfor `$" + to_string(Price) + " `wGems.\nYou have `$" + to_string(gembux) + " `wGems left.\n\n`5Received: ``" + getItemDef(ItemID).name + ""));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
							else {
								Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You can't afford `o" + getItemDef(ItemID).name + "``!  You're `$" + to_string(Price - gembux) + "`` Gems short."));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
						}
						if (cch == "action|buy\nitem|weather_arid\n") {
							/*Fast Item Setup*/
							auto Price = 10000;
							auto ItemID = 946;
							ifstream ifsz("save/gemdb/_" + pData->rawName + ".txt");
							string content((std::istreambuf_iterator<char>(ifsz)), (std::istreambuf_iterator<char>()));
							auto gembux = atoi(content.c_str());
							if (gembux >= Price) {
								if (CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && CheckItemExists(peer, ItemID) && CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && !CheckItemExists(peer, ItemID)) {
									Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
									GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You don't have enough space in your inventory that. You may be carrying to many of one of the items you are trying to purchase or you don't have enough free spaces to fit them all in your backpack!"));
									ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
									enet_peer_send(peer, 0, packet);
									delete p.data;
									break;
								}
								gembux -= Price;
								ofstream myfile;
								myfile.open("save/gemdb/_" + pData->rawName + ".txt");
								myfile << gembux;
								myfile.close();
								Player::OnSetBux(peer, gembux, 0);
								bool success = true;
								SaveItemMoreTimes(ItemID, 1, peer, success, "Purchased from store");
								Player::PlayAudio(peer, "audio/piano_nice.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You've purchased `o" + getItemDef(ItemID).name + " `wfor `$" + to_string(Price) + " `wGems.\nYou have `$" + to_string(gembux) + " `wGems left.\n\n`5Received: ``" + getItemDef(ItemID).name + ""));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
							else {
								Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You can't afford `o" + getItemDef(ItemID).name + "``!  You're `$" + to_string(Price - gembux) + "`` Gems short."));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
						}
						if (cch == "action|buy\nitem|weather_night\n") {
							/*Fast Item Setup*/
							auto Price = 10000;
							auto ItemID = 934;
							ifstream ifsz("save/gemdb/_" + pData->rawName + ".txt");
							string content((std::istreambuf_iterator<char>(ifsz)), (std::istreambuf_iterator<char>()));
							auto gembux = atoi(content.c_str());
							if (gembux >= Price) {
								if (CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && CheckItemExists(peer, ItemID) && CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && !CheckItemExists(peer, ItemID)) {
									Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
									GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You don't have enough space in your inventory that. You may be carrying to many of one of the items you are trying to purchase or you don't have enough free spaces to fit them all in your backpack!"));
									ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
									enet_peer_send(peer, 0, packet);
									delete p.data;
									break;
								}
								gembux -= Price;
								ofstream myfile;
								myfile.open("save/gemdb/_" + pData->rawName + ".txt");
								myfile << gembux;
								myfile.close();
								Player::OnSetBux(peer, gembux, 0);
								bool success = true;
								SaveItemMoreTimes(ItemID, 1, peer, success, "Purchased from store");
								Player::PlayAudio(peer, "audio/piano_nice.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You've purchased `o" + getItemDef(ItemID).name + " `wfor `$" + to_string(Price) + " `wGems.\nYou have `$" + to_string(gembux) + " `wGems left.\n\n`5Received: ``" + getItemDef(ItemID).name + ""));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
							else {
								Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You can't afford `o" + getItemDef(ItemID).name + "``!  You're `$" + to_string(Price - gembux) + "`` Gems short."));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
						}
						if (cch == "action|buy\nitem|weather_sunny\n") {
							/*Fast Item Setup*/
							auto Price = 1000;
							auto ItemID = 932;
							ifstream ifsz("save/gemdb/_" + pData->rawName + ".txt");
							string content((std::istreambuf_iterator<char>(ifsz)), (std::istreambuf_iterator<char>()));
							auto gembux = atoi(content.c_str());
							if (gembux >= Price) {
								if (CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && CheckItemExists(peer, ItemID) && CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && !CheckItemExists(peer, ItemID)) {
									Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
									GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You don't have enough space in your inventory that. You may be carrying to many of one of the items you are trying to purchase or you don't have enough free spaces to fit them all in your backpack!"));
									ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
									enet_peer_send(peer, 0, packet);
									delete p.data;
									break;
								}
								gembux -= Price;
								ofstream myfile;
								myfile.open("save/gemdb/_" + pData->rawName + ".txt");
								myfile << gembux;
								myfile.close();
								Player::OnSetBux(peer, gembux, 0);
								bool success = true;
								SaveItemMoreTimes(ItemID, 1, peer, success, "Purchased from store");
								Player::PlayAudio(peer, "audio/piano_nice.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You've purchased `o" + getItemDef(ItemID).name + " `wfor `$" + to_string(Price) + " `wGems.\nYou have `$" + to_string(gembux) + " `wGems left.\n\n`5Received: ``" + getItemDef(ItemID).name + ""));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
							else {
								Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You can't afford `o" + getItemDef(ItemID).name + "``!  You're `$" + to_string(Price - gembux) + "`` Gems short."));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
						}
						if (cch == "action|buy\nitem|safe_vault\n") {
							/*Fast Item Setup*/
							auto Price = 10000;
							auto ItemID = 8878;
							ifstream ifsz("save/gemdb/_" + pData->rawName + ".txt");
							string content((std::istreambuf_iterator<char>(ifsz)), (std::istreambuf_iterator<char>()));
							auto gembux = atoi(content.c_str());
							if (gembux >= Price) {
								if (CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && CheckItemExists(peer, ItemID) && CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && !CheckItemExists(peer, ItemID)) {
									Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
									GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You don't have enough space in your inventory that. You may be carrying to many of one of the items you are trying to purchase or you don't have enough free spaces to fit them all in your backpack!"));
									ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
									enet_peer_send(peer, 0, packet);
									delete p.data;
									break;
								}
								gembux -= Price;
								ofstream myfile;
								myfile.open("save/gemdb/_" + pData->rawName + ".txt");
								myfile << gembux;
								myfile.close();
								Player::OnSetBux(peer, gembux, 0);
								bool success = true;
								SaveItemMoreTimes(ItemID, 1, peer, success, "Purchased from store");
								Player::PlayAudio(peer, "audio/piano_nice.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You've purchased `o" + getItemDef(ItemID).name + " `wfor `$" + to_string(Price) + " `wGems.\nYou have `$" + to_string(gembux) + " `wGems left.\n\n`5Received: ``" + getItemDef(ItemID).name + ""));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
							else {
								Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You can't afford `o" + getItemDef(ItemID).name + "``!  You're `$" + to_string(Price - gembux) + "`` Gems short."));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
						}
						if (cch == "action|buy\nitem|bountiful_blast\n") {
							/*Fast Item Setup*/
							auto Price = 5000;
							auto ItemID = 8738;
							ifstream ifsz("save/gemdb/_" + pData->rawName + ".txt");
							string content((std::istreambuf_iterator<char>(ifsz)), (std::istreambuf_iterator<char>()));
							auto gembux = atoi(content.c_str());
							if (gembux >= Price) {
								if (CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && CheckItemExists(peer, ItemID) && CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && !CheckItemExists(peer, ItemID)) {
									Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
									GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You don't have enough space in your inventory that. You may be carrying to many of one of the items you are trying to purchase or you don't have enough free spaces to fit them all in your backpack!"));
									ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
									enet_peer_send(peer, 0, packet);
									delete p.data;
									break;
								}
								gembux -= Price;
								ofstream myfile;
								myfile.open("save/gemdb/_" + pData->rawName + ".txt");
								myfile << gembux;
								myfile.close();
								Player::OnSetBux(peer, gembux, 0);
								bool success = true;
								SaveItemMoreTimes(ItemID, 1, peer, success, "Purchased from store");
								Player::PlayAudio(peer, "audio/piano_nice.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You've purchased `o" + getItemDef(ItemID).name + " `wfor `$" + to_string(Price) + " `wGems.\nYou have `$" + to_string(gembux) + " `wGems left.\n\n`5Received: ``" + getItemDef(ItemID).name + ""));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
							else {
								Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You can't afford `o" + getItemDef(ItemID).name + "``!  You're `$" + to_string(Price - gembux) + "`` Gems short."));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
						}
						if (cch == "action|buy\nitem|surg_blast\n") {
							/*Fast Item Setup*/
							auto Price = 10000;
							auto ItemID = 8556;
							ifstream ifsz("save/gemdb/_" + pData->rawName + ".txt");
							string content((std::istreambuf_iterator<char>(ifsz)), (std::istreambuf_iterator<char>()));
							auto gembux = atoi(content.c_str());
							if (gembux >= Price) {
								if (CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && CheckItemExists(peer, ItemID) && CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && !CheckItemExists(peer, ItemID)) {
									Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
									GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You don't have enough space in your inventory that. You may be carrying to many of one of the items you are trying to purchase or you don't have enough free spaces to fit them all in your backpack!"));
									ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
									enet_peer_send(peer, 0, packet);
									delete p.data;
									break;
								}
								gembux -= Price;
								ofstream myfile;
								myfile.open("save/gemdb/_" + pData->rawName + ".txt");
								myfile << gembux;
								myfile.close();
								Player::OnSetBux(peer, gembux, 0);
								bool success = true;
								SaveItemMoreTimes(ItemID, 1, peer, success, "Purchased from store");
								Player::PlayAudio(peer, "audio/piano_nice.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You've purchased `o" + getItemDef(ItemID).name + " `wfor `$" + to_string(Price) + " `wGems.\nYou have `$" + to_string(gembux) + " `wGems left.\n\n`5Received: ``" + getItemDef(ItemID).name + ""));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
							else {
								Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You can't afford `o" + getItemDef(ItemID).name + "``!  You're `$" + to_string(Price - gembux) + "`` Gems short."));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
						}
						if (cch == "action|buy\nitem|starship_blast\n") {
							/*Fast Item Setup*/
							auto Price = 10000;
							auto ItemID = 6420;
							ifstream ifsz("save/gemdb/_" + pData->rawName + ".txt");
							string content((std::istreambuf_iterator<char>(ifsz)), (std::istreambuf_iterator<char>()));
							auto gembux = atoi(content.c_str());
							if (gembux >= Price) {
								if (CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && CheckItemExists(peer, ItemID) && CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && !CheckItemExists(peer, ItemID)) {
									Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
									GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You don't have enough space in your inventory that. You may be carrying to many of one of the items you are trying to purchase or you don't have enough free spaces to fit them all in your backpack!"));
									ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
									enet_peer_send(peer, 0, packet);
									delete p.data;
									break;
								}
								gembux -= Price;
								ofstream myfile;
								myfile.open("save/gemdb/_" + pData->rawName + ".txt");
								myfile << gembux;
								myfile.close();
								Player::OnSetBux(peer, gembux, 0);
								bool success = true;
								SaveItemMoreTimes(ItemID, 1, peer, success, "Purchased from store");
								Player::PlayAudio(peer, "audio/piano_nice.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You've purchased `o" + getItemDef(ItemID).name + " `wfor `$" + to_string(Price) + " `wGems.\nYou have `$" + to_string(gembux) + " `wGems left.\n\n`5Received: ``" + getItemDef(ItemID).name + ""));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
							else {
								Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You can't afford `o" + getItemDef(ItemID).name + "``!  You're `$" + to_string(Price - gembux) + "`` Gems short."));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
						}
						if (cch == "action|buy\nitem|change_addr\n") {
							/*Fast Item Setup*/
							auto Price = 20000;
							auto ItemID = 2580;
							ifstream ifsz("save/gemdb/_" + pData->rawName + ".txt");
							string content((std::istreambuf_iterator<char>(ifsz)), (std::istreambuf_iterator<char>()));
							auto gembux = atoi(content.c_str());
							if (gembux >= Price) {
								if (CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && CheckItemExists(peer, ItemID) && CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && !CheckItemExists(peer, ItemID)) {
									Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
									GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You don't have enough space in your inventory that. You may be carrying to many of one of the items you are trying to purchase or you don't have enough free spaces to fit them all in your backpack!"));
									ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
									enet_peer_send(peer, 0, packet);
									delete p.data;
									break;
								}
								gembux -= Price;
								ofstream myfile;
								myfile.open("save/gemdb/_" + pData->rawName + ".txt");
								myfile << gembux;
								myfile.close();
								Player::OnSetBux(peer, gembux, 0);
								bool success = true;
								SaveItemMoreTimes(ItemID, 1, peer, success, "Purchased from store");
								Player::PlayAudio(peer, "audio/piano_nice.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You've purchased `o" + getItemDef(ItemID).name + " `wfor `$" + to_string(Price) + " `wGems.\nYou have `$" + to_string(gembux) + " `wGems left.\n\n`5Received: ``" + getItemDef(ItemID).name + ""));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
							else {
								Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You can't afford `o" + getItemDef(ItemID).name + "``!  You're `$" + to_string(Price - gembux) + "`` Gems short."));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
						}
						if (cch == "action|buy\nitem|checkout_counter\n") {
							/*Fast Item Setup*/
							auto Price = 50000;
							auto ItemID = 9270;
							ifstream ifsz("save/gemdb/_" + pData->rawName + ".txt");
							string content((std::istreambuf_iterator<char>(ifsz)), (std::istreambuf_iterator<char>()));
							auto gembux = atoi(content.c_str());
							if (gembux >= Price) {
								if (CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && CheckItemExists(peer, ItemID) && CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && !CheckItemExists(peer, ItemID)) {
									Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
									GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You don't have enough space in your inventory that. You may be carrying to many of one of the items you are trying to purchase or you don't have enough free spaces to fit them all in your backpack!"));
									ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
									enet_peer_send(peer, 0, packet);
									delete p.data;
									break;
								}
								gembux -= Price;
								ofstream myfile;
								myfile.open("save/gemdb/_" + pData->rawName + ".txt");
								myfile << gembux;
								myfile.close();
								Player::OnSetBux(peer, gembux, 0);
								bool success = true;
								SaveItemMoreTimes(ItemID, 1, peer, success, "Purchased from store");
								Player::PlayAudio(peer, "audio/piano_nice.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You've purchased `o" + getItemDef(ItemID).name + " `wfor `$" + to_string(Price) + " `wGems.\nYou have `$" + to_string(gembux) + " `wGems left.\n\n`5Received: ``" + getItemDef(ItemID).name + ""));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
							else {
								Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You can't afford `o" + getItemDef(ItemID).name + "``!  You're `$" + to_string(Price - gembux) + "`` Gems short."));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
						}
						if (cch == "action|buy\nitem|digi_vend\n") {
							/*Fast Item Setup*/
							auto Price = 12000;
							auto ItemID = 9268;
							ifstream ifsz("save/gemdb/_" + pData->rawName + ".txt");
							string content((std::istreambuf_iterator<char>(ifsz)), (std::istreambuf_iterator<char>()));
							auto gembux = atoi(content.c_str());
							if (gembux >= Price) {
								if (CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && CheckItemExists(peer, ItemID) && CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && !CheckItemExists(peer, ItemID)) {
									Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
									GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You don't have enough space in your inventory that. You may be carrying to many of one of the items you are trying to purchase or you don't have enough free spaces to fit them all in your backpack!"));
									ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
									enet_peer_send(peer, 0, packet);
									delete p.data;
									break;
								}
								gembux -= Price;
								ofstream myfile;
								myfile.open("save/gemdb/_" + pData->rawName + ".txt");
								myfile << gembux;
								myfile.close();
								Player::OnSetBux(peer, gembux, 0);
								bool success = true;
								SaveItemMoreTimes(ItemID, 1, peer, success, "Purchased from store");
								Player::PlayAudio(peer, "audio/piano_nice.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You've purchased `o" + getItemDef(ItemID).name + " `wfor `$" + to_string(Price) + " `wGems.\nYou have `$" + to_string(gembux) + " `wGems left.\n\n`5Received: ``" + getItemDef(ItemID).name + ""));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
							else {
								Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You can't afford `o" + getItemDef(ItemID).name + "``!  You're `$" + to_string(Price - gembux) + "`` Gems short."));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
						}
						if (cch == "action|buy\nitem|door_mover\n") {
							/*Fast Item Setup*/
							auto Price = 5000;
							auto ItemID = 1404;
							ifstream ifsz("save/gemdb/_" + pData->rawName + ".txt");
							string content((std::istreambuf_iterator<char>(ifsz)), (std::istreambuf_iterator<char>()));
							auto gembux = atoi(content.c_str());
							if (gembux >= Price) {
								if (CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && CheckItemExists(peer, ItemID) && CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && !CheckItemExists(peer, ItemID)) {
									Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
									GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You don't have enough space in your inventory that. You may be carrying to many of one of the items you are trying to purchase or you don't have enough free spaces to fit them all in your backpack!"));
									ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
									enet_peer_send(peer, 0, packet);
									delete p.data;
									break;
								}
								gembux -= Price;
								ofstream myfile;
								myfile.open("save/gemdb/_" + pData->rawName + ".txt");
								myfile << gembux;
								myfile.close();
								Player::OnSetBux(peer, gembux, 0);
								bool success = true;
								SaveItemMoreTimes(ItemID, 1, peer, success, "Purchased from store");
								Player::PlayAudio(peer, "audio/piano_nice.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You've purchased `o" + getItemDef(ItemID).name + " `wfor `$" + to_string(Price) + " `wGems.\nYou have `$" + to_string(gembux) + " `wGems left.\n\n`5Received: ``" + getItemDef(ItemID).name + ""));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
							else {
								Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You can't afford `o" + getItemDef(ItemID).name + "``!  You're `$" + to_string(Price - gembux) + "`` Gems short."));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
						}
						if (cch == "action|buy\nitem|world_lock\n") {
							/*Fast Item Setup*/
							auto Price = 2000;
							auto ItemID = 242;
							ifstream ifsz("save/gemdb/_" + pData->rawName + ".txt");
							string content((std::istreambuf_iterator<char>(ifsz)), (std::istreambuf_iterator<char>()));
							auto gembux = atoi(content.c_str());
							if (gembux >= Price) {
								if (CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && CheckItemExists(peer, ItemID) && CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && !CheckItemExists(peer, ItemID)) {
									Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
									GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You don't have enough space in your inventory that. You may be carrying to many of one of the items you are trying to purchase or you don't have enough free spaces to fit them all in your backpack!"));
									ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
									enet_peer_send(peer, 0, packet);
									delete p.data;
									break;
								}
								gembux -= Price;
								ofstream myfile;
								myfile.open("save/gemdb/_" + pData->rawName + ".txt");
								myfile << gembux;
								myfile.close();
								Player::OnSetBux(peer, gembux, 0);
								bool success = true;
								SaveItemMoreTimes(ItemID, 1, peer, success, "Purchased from store");
								Player::PlayAudio(peer, "audio/piano_nice.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You've purchased `o" + getItemDef(ItemID).name + " `wfor `$" + to_string(Price) + " `wGems.\nYou have `$" + to_string(gembux) + " `wGems left.\n\n`5Received: ``" + getItemDef(ItemID).name + ""));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
							else {
								Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You can't afford `o" + getItemDef(ItemID).name + "``!  You're `$" + to_string(Price - gembux) + "`` Gems short."));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
						}
						if (cch == "action|buy\nitem|world_lock_10_pack\n") {
							auto Price = 20000;
							auto ItemID = 242;
							ifstream ifsz("save/gemdb/_" + pData->rawName + ".txt");
							string content((std::istreambuf_iterator<char>(ifsz)), (std::istreambuf_iterator<char>()));
							auto gembux = atoi(content.c_str());
							if (gembux >= Price) {
								if (CheckItemMaxed(peer, ItemID, 10) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && CheckItemExists(peer, ItemID) && CheckItemMaxed(peer, ItemID, 10) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && !CheckItemExists(peer, ItemID)) {
									Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
									GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You don't have enough space in your inventory that. You may be carrying to many of one of the items you are trying to purchase or you don't have enough free spaces to fit them all in your backpack!"));
									ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
									enet_peer_send(peer, 0, packet);
									delete p.data;
									break;
								}
								gembux -= Price;
								ofstream myfile;
								myfile.open("save/gemdb/_" + pData->rawName + ".txt");
								myfile << gembux;
								myfile.close();
								Player::OnSetBux(peer, gembux, 0);
								bool success = true;
								SaveItemMoreTimes(ItemID, 10, peer, success, "Purchased from store");
								Player::PlayAudio(peer, "audio/piano_nice.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You've purchased `o10 " + getItemDef(ItemID).name + " Pack `wfor `$" + to_string(Price) + " `wGems.\nYou have `$" + to_string(gembux) + " `wGems left.\n\n`5Received: ``10 " + getItemDef(ItemID).name + ""));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
							else {
								Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You can't afford `o10 " + getItemDef(ItemID).name + " Pack``!  You're `$" + to_string(Price - gembux) + "`` Gems short."));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
						}
						if (cch == "action|buy\nitem|upgrade_backpack\n") {
							/*Fast Item Setup*/
							auto Price = 10000;
							ifstream ifsz("save/gemdb/_" + pData->rawName + ".txt");
							string content((std::istreambuf_iterator<char>(ifsz)), (std::istreambuf_iterator<char>()));
							auto gembux = atoi(content.c_str());
							if (gembux >= Price) {
								if (pData->currentInventorySize >= 200) break;
								gembux -= Price;
								ofstream myfile;
								myfile.open("save/gemdb/_" + pData->rawName + ".txt");
								myfile << gembux;
								myfile.close();
								Player::OnSetBux(peer, gembux, 0);
								Player::PlayAudio(peer, "audio/piano_nice.wav", 0);
								short nextSpace = 0;
								if (pData->currentInventorySize + 10 > 200) {
									nextSpace = 200;
								}
								else {
									nextSpace = pData->currentInventorySize + 10;
								}
								pData->currentInventorySize += 10;
								SendInventory(peer, pData->inventory);
								const auto p3 = packetEnd(appendFloat(appendIntx(appendFloat(appendFloat(appendFloat(appendString(createPacket(), "OnSetClothing"), (static_cast<PlayerInfo*>(peer->data))->cloth_hair, (static_cast<PlayerInfo*>(peer->data))->cloth_shirt, (static_cast<PlayerInfo*>(peer->data))->cloth_pants), (static_cast<PlayerInfo*>(peer->data))->cloth_feet, (static_cast<PlayerInfo*>(peer->data))->cloth_face, (static_cast<PlayerInfo*>(peer->data))->cloth_hand), (static_cast<PlayerInfo*>(peer->data))->cloth_back, (static_cast<PlayerInfo*>(peer->data))->cloth_mask, (static_cast<PlayerInfo*>(peer->data))->cloth_necklace), (static_cast<PlayerInfo*>(peer->data))->skinColor), (static_cast<PlayerInfo*>(peer->data))->cloth_ances, 0.0f, 0.0f));
								memcpy(p3.data + 8, &((static_cast<PlayerInfo*>(peer->data))->netID), 4);
								const auto packet = enet_packet_create(p3.data, p3.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You've purchased `0Upgrade Backpack`` (`w10 Slots``) `wfor `$" + to_string(Price) + " `wGems.\nYou have `$" + to_string(gembux) + " `wGems left.\n\n`5Received: ``Backpack Upgrade"));
								ENetPacket* packets = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packets);
								delete p.data;
							}
							else {
								Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You can't afford `0Upgrade Backpack`` (`w10 Slots``)``!  You're `$" + to_string(Price - gembux) + "`` Gems short."));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
						}
						if (cch == "action|buy\nitem|door_pack\n") {
							ifstream ifsz("save/gemdb/_" + pData->rawName + ".txt");
							string content((std::istreambuf_iterator<char>(ifsz)), (std::istreambuf_iterator<char>()));
							auto gembux = atoi(content.c_str());
							if (gembux >= 15) {
								if (CheckItemMaxed(peer, 12, 1) || CheckItemMaxed(peer, 20, 1) || pData->inventory.items.size() + 2 >= pData->currentInventorySize) {
									Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
									GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You don't have enough space in your inventory that. You may be carrying to many of one of the items you are trying to purchase or you don't have enough free spaces to fit them all in your backpack!"));
									ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
									enet_peer_send(peer, 0, packet);
									delete p.data;
									break;
								}
								gembux -= 15;
								ofstream myfile;
								myfile.open("save/gemdb/_" + pData->rawName + ".txt");
								myfile << gembux;
								myfile.close();
								Player::OnSetBux(peer, gembux, 0);
								bool success = true;
								SaveItemMoreTimes(12, 1, peer, success, "Purchased from store");
								SaveItemMoreTimes(20, 1, peer, success, "Purchased from store");
								Player::PlayAudio(peer, "audio/piano_nice.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You've purchased `oHello Pack `wfor `$" + to_string(15) + " `wGems.\nYou have `$" + to_string(gembux) + " `wGems left.\n\n`5Received: ``Door`$,`` Sign\n"));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
							else {
								Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You can't afford `oHello Pack``!  You're `$" + to_string(15 - gembux) + "`` Gems short."));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
						}
						if (cch == "action|buy\nitem|paintbrush\n") {
							ifstream ifsz("save/gemdb/_" + pData->rawName + ".txt");
							string content((std::istreambuf_iterator<char>(ifsz)), (std::istreambuf_iterator<char>()));
							auto gembux = atoi(content.c_str());
							if (gembux >= 30000) {
								if (CheckItemMaxed(peer, 3494, 1) || CheckItemMaxed(peer, 3478, 5) || CheckItemMaxed(peer, 3480, 5) || CheckItemMaxed(peer, 3482, 5) || CheckItemMaxed(peer, 3484, 5) || CheckItemMaxed(peer, 3486, 5) || CheckItemMaxed(peer, 3488, 5) || CheckItemMaxed(peer, 3490, 5) || CheckItemMaxed(peer, 3492, 5) || pData->inventory.items.size() + 9 >= pData->currentInventorySize) {
									Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
									GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You don't have enough space in your inventory that. You may be carrying to many of one of the items you are trying to purchase or you don't have enough free spaces to fit them all in your backpack!"));
									ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
									enet_peer_send(peer, 0, packet);
									delete p.data;
									break;
								}
								gembux -= 30000;
								ofstream myfile;
								myfile.open("save/gemdb/_" + pData->rawName + ".txt");
								myfile << gembux;
								myfile.close();
								Player::OnSetBux(peer, gembux, 0);
								bool success = true;
								SaveItemMoreTimes(3494, 1, peer, success, "Purchased from store");
								SaveItemMoreTimes(3478, 5, peer, success, "Purchased from store");
								SaveItemMoreTimes(3480, 5, peer, success, "Purchased from store");
								SaveItemMoreTimes(3482, 5, peer, success, "Purchased from store");
								SaveItemMoreTimes(3484, 5, peer, success, "Purchased from store");
								SaveItemMoreTimes(3486, 5, peer, success, "Purchased from store");
								SaveItemMoreTimes(3488, 5, peer, success, "Purchased from store");
								SaveItemMoreTimes(3490, 5, peer, success, "Purchased from store");
								SaveItemMoreTimes(3492, 5, peer, success, "Purchased from store");
								Player::PlayAudio(peer, "audio/piano_nice.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You've purchased `oPainter's Pack `wfor `$" + to_string(30000) + " `wGems.\nYou have `$" + to_string(gembux) + " `wGems left.\n\n`5Received: ``Paintbrush`$,`` 5 Paint Bucket - Yellow`$,`` 5 Paint Bucket - Green`$,`` 5 Paint Bucket - Blue`$,`` 5 Paint Bucket - Varnish`$,`` 5 Paint Bucket - Red`$,`` 5 Paint Bucket - Charcoal`$,`` 5 Paint Bucket - Purple`$,`` 5 Paint Bucket - Aqua\n"));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
							else {
								Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You can't afford `oPainter's Pack``!  You're `$" + to_string(30000 - gembux) + "`` Gems short."));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
						}
						if (cch == "action|buy\nitem|vegas_pack\n") {
							ifstream ifsz("save/gemdb/_" + pData->rawName + ".txt");
							string content((std::istreambuf_iterator<char>(ifsz)), (std::istreambuf_iterator<char>()));
							auto gembux = atoi(content.c_str());
							if (gembux >= 20000) {
								if (CheckItemMaxed(peer, 740, 10) || CheckItemMaxed(peer, 744, 1) || CheckItemMaxed(peer, 1582, 1) || CheckItemMaxed(peer, 752, 4) || CheckItemMaxed(peer, 456, 1) || CheckItemMaxed(peer, 754, 1) || CheckItemMaxed(peer, 756, 1) || CheckItemMaxed(peer, 758, 1) || CheckItemMaxed(peer, 794, 1) || CheckItemMaxed(peer, 796, 1) || CheckItemMaxed(peer, 798, 1) || pData->inventory.items.size() + 11 >= pData->currentInventorySize) {
									Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
									GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You don't have enough space in your inventory that. You may be carrying to many of one of the items you are trying to purchase or you don't have enough free spaces to fit them all in your backpack!"));
									ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
									enet_peer_send(peer, 0, packet);
									delete p.data;
									break;
								}
								gembux -= 20000;
								ofstream myfile;
								myfile.open("save/gemdb/_" + pData->rawName + ".txt");
								myfile << gembux;
								myfile.close();
								Player::OnSetBux(peer, gembux, 0);
								bool success = true;
								SaveItemMoreTimes(740, 10, peer, success, "Purchased from store");
								SaveItemMoreTimes(744, 1, peer, success, "Purchased from store");
								SaveItemMoreTimes(1582, 1, peer, success, "Purchased from store");
								SaveItemMoreTimes(752, 4, peer, success, "Purchased from store");
								SaveItemMoreTimes(456, 1, peer, success, "Purchased from store");
								SaveItemMoreTimes(754, 1, peer, success, "Purchased from store");
								SaveItemMoreTimes(756, 1, peer, success, "Purchased from store");
								SaveItemMoreTimes(758, 1, peer, success, "Purchased from store");
								SaveItemMoreTimes(794, 1, peer, success, "Purchased from store");
								SaveItemMoreTimes(796, 1, peer, success, "Purchased from store");
								SaveItemMoreTimes(798, 1, peer, success, "Purchased from store");
								Player::PlayAudio(peer, "audio/piano_nice.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You've purchased `oVegas Pack `wfor `$" + to_string(20000) + " `wGems.\nYou have `$" + to_string(gembux) + " `wGems left.\n\n`5Received: ``10 Neon Lights`$,`` Card Block Seed`$,`` Pink Cadillac`$,`` 4 Flipping Coin`$,`` Dice Block`$,`` Gambler's Visor`$,`` Slot Machine`$,`` Roulette Wheel`$,`` Showgirl Headdress`$,`` Showgirl Top`$,`` Showgirl Leggings\n"));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
							else {
								Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You can't afford `oVegas Pack``!  You're `$" + to_string(20000 - gembux) + "`` Gems short."));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
						}
						if (cch == "action|buy\nitem|thermo_blast\n") {
							/*Fast Item Setup*/
							auto Price = 15000;
							auto ItemID = 1402;
							auto count = 1;
							ifstream ifsz("save/gemdb/_" + pData->rawName + ".txt");
							string content((std::istreambuf_iterator<char>(ifsz)), (std::istreambuf_iterator<char>()));
							auto gembux = atoi(content.c_str());
							if (gembux >= Price) {
								if (CheckItemMaxed(peer, ItemID, count) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && CheckItemExists(peer, ItemID) && CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && !CheckItemExists(peer, ItemID)) {
									Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
									GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You don't have enough space in your inventory that. You may be carrying to many of one of the items you are trying to purchase or you don't have enough free spaces to fit them all in your backpack!"));
									ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
									enet_peer_send(peer, 0, packet);
									delete p.data;
									break;
								}
								gembux -= Price;
								ofstream myfile;
								myfile.open("save/gemdb/_" + pData->rawName + ".txt");
								myfile << gembux;
								myfile.close();
								Player::OnSetBux(peer, gembux, 0);
								bool success = true;
								SaveItemMoreTimes(ItemID, count, peer, success, "Purchased from store");
								Player::PlayAudio(peer, "audio/piano_nice.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You've purchased " + to_string(count) + " `o" + getItemDef(ItemID).name + " `wfor `$" + to_string(Price) + " `wGems.\nYou have `$" + to_string(gembux) + " `wGems left.\n\n`5Received: ``" + to_string(count) + " " + getItemDef(ItemID).name + ""));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
							else {
								Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You can't afford `o" + getItemDef(ItemID).name + "``!  You're `$" + to_string(Price - gembux) + "`` Gems short."));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
						}
						if (cch == "action|buy\nitem|seils_magic_orb\n") {
							/*Fast Item Setup*/
							auto Price = 200;
							auto ItemID = 820;
							auto count = 1;
							auto contains = false;
							auto KiekTuri = 0;
							try {
								for (auto i = 0; i < pData->inventory.items.size(); i++) {
									if (pData->inventory.items.at(i).itemID == 1486 && pData->inventory.items.at(i).itemCount >= 1) {
										KiekTuri = pData->inventory.items.at(i).itemCount;
										break;
									}
								}
							}
							catch (const std::out_of_range& e) {
								std::cout << e.what() << std::endl;
							}
							SearchInventoryItem(peer, 1486, Price, contains);
							if (contains) {
								if (CheckItemMaxed(peer, ItemID, count) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && CheckItemExists(peer, ItemID) && CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && !CheckItemExists(peer, ItemID)) {
									Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
									GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You don't have enough space in your inventory that. You may be carrying to many of one of the items you are trying to purchase or you don't have enough free spaces to fit them all in your backpack!"));
									ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
									enet_peer_send(peer, 0, packet);
									delete p.data;
									break;
								}
								RemoveInventoryItem(1486, Price, peer, true);
								bool success = true;
								SaveItemMoreTimes(ItemID, count, peer, success, "Purchased from store");
								Player::PlayAudio(peer, "audio/piano_nice.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You've purchased " + to_string(count) + " `o" + getItemDef(ItemID).name + " `wfor `$" + to_string(Price) + " `wGrowtokens.\nYou have `$" + to_string(KiekTuri - Price) + " `wGrowtokens left.\n\n`5Received: ``" + to_string(count) + " " + getItemDef(ItemID).name + ""));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
							else {
								Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You can't afford `o" + getItemDef(ItemID).name + "``!  You're `$" + to_string(Price - KiekTuri) + "`` Growtokens short."));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
						}
						if (cch == "action|buy\nitem|atomic_shadow_scythe\n") {
							/*Fast Item Setup*/
							auto Price = 200;
							auto ItemID = 1484;
							auto count = 1;
							auto contains = false;
							auto KiekTuri = 0;
							try {
								for (auto i = 0; i < pData->inventory.items.size(); i++) {
									if (pData->inventory.items.at(i).itemID == 1486 && pData->inventory.items.at(i).itemCount >= 1) {
										KiekTuri = pData->inventory.items.at(i).itemCount;
										break;
									}
								}
							}
							catch (const std::out_of_range& e) {
								std::cout << e.what() << std::endl;
							}
							SearchInventoryItem(peer, 1486, Price, contains);
							if (contains) {
								if (CheckItemMaxed(peer, ItemID, count) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && CheckItemExists(peer, ItemID) && CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && !CheckItemExists(peer, ItemID)) {
									Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
									GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You don't have enough space in your inventory that. You may be carrying to many of one of the items you are trying to purchase or you don't have enough free spaces to fit them all in your backpack!"));
									ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
									enet_peer_send(peer, 0, packet);
									delete p.data;
									break;
								}
								RemoveInventoryItem(1486, Price, peer, true);
								bool success = true;
								SaveItemMoreTimes(ItemID, count, peer, success, "Purchased from store");
								Player::PlayAudio(peer, "audio/piano_nice.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You've purchased " + to_string(count) + " `o" + getItemDef(ItemID).name + " `wfor `$" + to_string(Price) + " `wGrowtokens.\nYou have `$" + to_string(KiekTuri - Price) + " `wGrowtokens left.\n\n`5Received: ``" + to_string(count) + " " + getItemDef(ItemID).name + ""));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
							else {
								Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You can't afford `o" + getItemDef(ItemID).name + "``!  You're `$" + to_string(Price - KiekTuri) + "`` Growtokens short."));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
						}
						if (cch == "action|buy\nitem|zerkon_helmet\n") {
							/*Fast Item Setup*/
							auto Price = 200;
							auto ItemID = 1440;
							auto count = 1;
							auto contains = false;
							auto KiekTuri = 0;
							try {
								for (auto i = 0; i < pData->inventory.items.size(); i++) {
									if (pData->inventory.items.at(i).itemID == 1486 && pData->inventory.items.at(i).itemCount >= 1) {
										KiekTuri = pData->inventory.items.at(i).itemCount;
										break;
									}
								}
							}
							catch (const std::out_of_range& e) {
								std::cout << e.what() << std::endl;
							}
							SearchInventoryItem(peer, 1486, Price, contains);
							if (contains) {
								if (CheckItemMaxed(peer, ItemID, count) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && CheckItemExists(peer, ItemID) && CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && !CheckItemExists(peer, ItemID)) {
									Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
									GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You don't have enough space in your inventory that. You may be carrying to many of one of the items you are trying to purchase or you don't have enough free spaces to fit them all in your backpack!"));
									ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
									enet_peer_send(peer, 0, packet);
									delete p.data;
									break;
								}
								RemoveInventoryItem(1486, Price, peer, true);
								bool success = true;
								SaveItemMoreTimes(ItemID, count, peer, success, "Purchased from store");
								Player::PlayAudio(peer, "audio/piano_nice.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You've purchased " + to_string(count) + " `o" + getItemDef(ItemID).name + " `wfor `$" + to_string(Price) + " `wGrowtokens.\nYou have `$" + to_string(KiekTuri - Price) + " `wGrowtokens left.\n\n`5Received: ``" + to_string(count) + " " + getItemDef(ItemID).name + ""));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
							else {
								Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You can't afford `o" + getItemDef(ItemID).name + "``!  You're `$" + to_string(Price - KiekTuri) + "`` Growtokens short."));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
						}
						if (cch == "action|buy\nitem|frosty_hair\n") {
							/*Fast Item Setup*/
							auto Price = 200;
							auto ItemID = 1444;
							auto count = 1;
							auto contains = false;
							auto KiekTuri = 0;
							try {
								for (auto i = 0; i < pData->inventory.items.size(); i++) {
									if (pData->inventory.items.at(i).itemID == 1486 && pData->inventory.items.at(i).itemCount >= 1) {
										KiekTuri = pData->inventory.items.at(i).itemCount;
										break;
									}
								}
							}
							catch (const std::out_of_range& e) {
								std::cout << e.what() << std::endl;
							}
							SearchInventoryItem(peer, 1486, Price, contains);
							if (contains) {
								if (CheckItemMaxed(peer, ItemID, count) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && CheckItemExists(peer, ItemID) && CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && !CheckItemExists(peer, ItemID)) {
									Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
									GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You don't have enough space in your inventory that. You may be carrying to many of one of the items you are trying to purchase or you don't have enough free spaces to fit them all in your backpack!"));
									ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
									enet_peer_send(peer, 0, packet);
									delete p.data;
									break;
								}
								RemoveInventoryItem(1486, Price, peer, true);
								bool success = true;
								SaveItemMoreTimes(ItemID, count, peer, success, "Purchased from store");
								Player::PlayAudio(peer, "audio/piano_nice.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You've purchased " + to_string(count) + " `o" + getItemDef(ItemID).name + " `wfor `$" + to_string(Price) + " `wGrowtokens.\nYou have `$" + to_string(KiekTuri - Price) + " `wGrowtokens left.\n\n`5Received: ``" + to_string(count) + " " + getItemDef(ItemID).name + ""));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
							else {
								Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You can't afford `o" + getItemDef(ItemID).name + "``!  You're `$" + to_string(Price - KiekTuri) + "`` Growtokens short."));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
						}
						if (cch == "action|buy\nitem|meow_ears\n") {
							/*Fast Item Setup*/
							auto Price = 200;
							auto ItemID = 698;
							auto count = 1;
							auto contains = false;
							auto KiekTuri = 0;
							try {
								for (auto i = 0; i < pData->inventory.items.size(); i++) {
									if (pData->inventory.items.at(i).itemID == 1486 && pData->inventory.items.at(i).itemCount >= 1) {
										KiekTuri = pData->inventory.items.at(i).itemCount;
										break;
									}
								}
							}
							catch (const std::out_of_range& e) {
								std::cout << e.what() << std::endl;
							}
							SearchInventoryItem(peer, 1486, Price, contains);
							if (contains) {
								if (CheckItemMaxed(peer, ItemID, count) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && CheckItemExists(peer, ItemID) && CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && !CheckItemExists(peer, ItemID)) {
									Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
									GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You don't have enough space in your inventory that. You may be carrying to many of one of the items you are trying to purchase or you don't have enough free spaces to fit them all in your backpack!"));
									ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
									enet_peer_send(peer, 0, packet);
									delete p.data;
									break;
								}
								RemoveInventoryItem(1486, Price, peer, true);
								bool success = true;
								SaveItemMoreTimes(ItemID, count, peer, success, "Purchased from store");
								Player::PlayAudio(peer, "audio/piano_nice.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You've purchased " + to_string(count) + " `o" + getItemDef(ItemID).name + " `wfor `$" + to_string(Price) + " `wGrowtokens.\nYou have `$" + to_string(KiekTuri - Price) + " `wGrowtokens left.\n\n`5Received: ``" + to_string(count) + " " + getItemDef(ItemID).name + ""));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
							else {
								Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You can't afford `o" + getItemDef(ItemID).name + "``!  You're `$" + to_string(Price - KiekTuri) + "`` Growtokens short."));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
						}
						if (cch == "action|buy\nitem|puppy_leash\n") {
							/*Fast Item Setup*/
							auto Price = 200;
							auto ItemID = 1742;
							auto count = 1;
							auto contains = false;
							auto KiekTuri = 0;
							try {
								for (auto i = 0; i < pData->inventory.items.size(); i++) {
									if (pData->inventory.items.at(i).itemID == 1486 && pData->inventory.items.at(i).itemCount >= 1) {
										KiekTuri = pData->inventory.items.at(i).itemCount;
										break;
									}
								}
							}
							catch (const std::out_of_range& e) {
								std::cout << e.what() << std::endl;
							}
							SearchInventoryItem(peer, 1486, Price, contains);
							if (contains) {
								if (CheckItemMaxed(peer, ItemID, count) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && CheckItemExists(peer, ItemID) && CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && !CheckItemExists(peer, ItemID)) {
									Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
									GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You don't have enough space in your inventory that. You may be carrying to many of one of the items you are trying to purchase or you don't have enough free spaces to fit them all in your backpack!"));
									ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
									enet_peer_send(peer, 0, packet);
									delete p.data;
									break;
								}
								RemoveInventoryItem(1486, Price, peer, true);
								bool success = true;
								SaveItemMoreTimes(ItemID, count, peer, success, "Purchased from store");
								Player::PlayAudio(peer, "audio/piano_nice.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You've purchased " + to_string(count) + " `o" + getItemDef(ItemID).name + " `wfor `$" + to_string(Price) + " `wGrowtokens.\nYou have `$" + to_string(KiekTuri - Price) + " `wGrowtokens left.\n\n`5Received: ``" + to_string(count) + " " + getItemDef(ItemID).name + ""));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
							else {
								Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You can't afford `o" + getItemDef(ItemID).name + "``!  You're `$" + to_string(Price - KiekTuri) + "`` Growtokens short."));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
						}
						if (cch == "action|buy\nitem|cat_eyes\n") {
							/*Fast Item Setup*/
							auto Price = 100;
							auto ItemID = 7106;
							auto count = 1;
							auto contains = false;
							auto KiekTuri = 0;
							try {
								for (auto i = 0; i < pData->inventory.items.size(); i++) {
									if (pData->inventory.items.at(i).itemID == 1486 && pData->inventory.items.at(i).itemCount >= 1) {
										KiekTuri = pData->inventory.items.at(i).itemCount;
										break;
									}
								}
							}
							catch (const std::out_of_range& e) {
								std::cout << e.what() << std::endl;
							}
							SearchInventoryItem(peer, 1486, Price, contains);
							if (contains) {
								if (CheckItemMaxed(peer, ItemID, count) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && CheckItemExists(peer, ItemID) && CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && !CheckItemExists(peer, ItemID)) {
									Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
									GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You don't have enough space in your inventory that. You may be carrying to many of one of the items you are trying to purchase or you don't have enough free spaces to fit them all in your backpack!"));
									ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
									enet_peer_send(peer, 0, packet);
									delete p.data;
									break;
								}
								RemoveInventoryItem(1486, Price, peer, true);
								bool success = true;
								SaveItemMoreTimes(ItemID, count, peer, success, "Purchased from store");
								Player::PlayAudio(peer, "audio/piano_nice.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You've purchased " + to_string(count) + " `o" + getItemDef(ItemID).name + " `wfor `$" + to_string(Price) + " `wGrowtokens.\nYou have `$" + to_string(KiekTuri - Price) + " `wGrowtokens left.\n\n`5Received: ``" + to_string(count) + " " + getItemDef(ItemID).name + ""));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
							else {
								Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You can't afford `o" + getItemDef(ItemID).name + "``!  You're `$" + to_string(Price - KiekTuri) + "`` Growtokens short."));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
						}
						if (cch == "action|buy\nitem|really_dangerous_pet_llama\n") {
							/*Fast Item Setup*/
							auto Price = 200;
							auto ItemID = 10050;
							auto count = 1;
							auto contains = false;
							auto KiekTuri = 0;
							try {
								for (auto i = 0; i < pData->inventory.items.size(); i++) {
									if (pData->inventory.items.at(i).itemID == 1486 && pData->inventory.items.at(i).itemCount >= 1) {
										KiekTuri = pData->inventory.items.at(i).itemCount;
										break;
									}
								}
							}
							catch (const std::out_of_range& e) {
								std::cout << e.what() << std::endl;
							}
							SearchInventoryItem(peer, 1486, Price, contains);
							if (contains) {
								if (CheckItemMaxed(peer, ItemID, count) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && CheckItemExists(peer, ItemID) && CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && !CheckItemExists(peer, ItemID)) {
									Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
									GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You don't have enough space in your inventory that. You may be carrying to many of one of the items you are trying to purchase or you don't have enough free spaces to fit them all in your backpack!"));
									ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
									enet_peer_send(peer, 0, packet);
									delete p.data;
									break;
								}
								RemoveInventoryItem(1486, Price, peer, true);
								bool success = true;
								SaveItemMoreTimes(ItemID, count, peer, success, "Purchased from store");
								Player::PlayAudio(peer, "audio/piano_nice.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You've purchased " + to_string(count) + " `o" + getItemDef(ItemID).name + " `wfor `$" + to_string(Price) + " `wGrowtokens.\nYou have `$" + to_string(KiekTuri - Price) + " `wGrowtokens left.\n\n`5Received: ``" + to_string(count) + " " + getItemDef(ItemID).name + ""));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
							else {
								Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You can't afford `o" + getItemDef(ItemID).name + "``!  You're `$" + to_string(Price - KiekTuri) + "`` Growtokens short."));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
						}
						if (cch == "action|buy\nitem|tsed\n") {
							/*Fast Item Setup*/
							auto Price = 150;
							auto ItemID = 10048;
							auto count = 1;
							auto contains = false;
							auto KiekTuri = 0;
							try {
								for (auto i = 0; i < pData->inventory.items.size(); i++) {
									if (pData->inventory.items.at(i).itemID == 1486 && pData->inventory.items.at(i).itemCount >= 1) {
										KiekTuri = pData->inventory.items.at(i).itemCount;
										break;
									}
								}
							}
							catch (const std::out_of_range& e) {
								std::cout << e.what() << std::endl;
							}
							SearchInventoryItem(peer, 1486, Price, contains);
							if (contains) {
								if (CheckItemMaxed(peer, ItemID, count) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && CheckItemExists(peer, ItemID) && CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && !CheckItemExists(peer, ItemID)) {
									Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
									GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You don't have enough space in your inventory that. You may be carrying to many of one of the items you are trying to purchase or you don't have enough free spaces to fit them all in your backpack!"));
									ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
									enet_peer_send(peer, 0, packet);
									delete p.data;
									break;
								}
								RemoveInventoryItem(1486, Price, peer, true);
								bool success = true;
								SaveItemMoreTimes(ItemID, count, peer, success, "Purchased from store");
								Player::PlayAudio(peer, "audio/piano_nice.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You've purchased " + to_string(count) + " `o" + getItemDef(ItemID).name + " `wfor `$" + to_string(Price) + " `wGrowtokens.\nYou have `$" + to_string(KiekTuri - Price) + " `wGrowtokens left.\n\n`5Received: ``" + to_string(count) + " " + getItemDef(ItemID).name + ""));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
							else {
								Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You can't afford `o" + getItemDef(ItemID).name + "``!  You're `$" + to_string(Price - KiekTuri) + "`` Growtokens short."));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
						}
						if (cch == "action|buy\nitem|grow_boy\n") {
							/*Fast Item Setup*/
							auto Price = 100;
							auto ItemID = 10046;
							auto count = 1;
							auto contains = false;
							auto KiekTuri = 0;
							try {
								for (auto i = 0; i < pData->inventory.items.size(); i++) {
									if (pData->inventory.items.at(i).itemID == 1486 && pData->inventory.items.at(i).itemCount >= 1) {
										KiekTuri = pData->inventory.items.at(i).itemCount;
										break;
									}
								}
							}
							catch (const std::out_of_range& e) {
								std::cout << e.what() << std::endl;
							}
							SearchInventoryItem(peer, 1486, Price, contains);
							if (contains) {
								if (CheckItemMaxed(peer, ItemID, count) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && CheckItemExists(peer, ItemID) && CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && !CheckItemExists(peer, ItemID)) {
									Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
									GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You don't have enough space in your inventory that. You may be carrying to many of one of the items you are trying to purchase or you don't have enough free spaces to fit them all in your backpack!"));
									ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
									enet_peer_send(peer, 0, packet);
									delete p.data;
									break;
								}
								RemoveInventoryItem(1486, Price, peer, true);
								bool success = true;
								SaveItemMoreTimes(ItemID, count, peer, success, "Purchased from store");
								Player::PlayAudio(peer, "audio/piano_nice.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You've purchased " + to_string(count) + " `o" + getItemDef(ItemID).name + " `wfor `$" + to_string(Price) + " `wGrowtokens.\nYou have `$" + to_string(KiekTuri - Price) + " `wGrowtokens left.\n\n`5Received: ``" + to_string(count) + " " + getItemDef(ItemID).name + ""));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
							else {
								Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You can't afford `o" + getItemDef(ItemID).name + "``!  You're `$" + to_string(Price - KiekTuri) + "`` Growtokens short."));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
						}
						if (cch == "action|buy\nitem|poseidon_diggers_trident\n") {
							/*Fast Item Setup*/
							auto Price = 200;
							auto ItemID = 7434;
							auto count = 1;
							auto contains = false;
							auto KiekTuri = 0;
							try {
								for (auto i = 0; i < pData->inventory.items.size(); i++) {
									if (pData->inventory.items.at(i).itemID == 1486 && pData->inventory.items.at(i).itemCount >= 1) {
										KiekTuri = pData->inventory.items.at(i).itemCount;
										break;
									}
								}
							}
							catch (const std::out_of_range& e) {
								std::cout << e.what() << std::endl;
							}
							SearchInventoryItem(peer, 1486, Price, contains);
							if (contains) {
								if (CheckItemMaxed(peer, ItemID, count) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && CheckItemExists(peer, ItemID) && CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && !CheckItemExists(peer, ItemID)) {
									Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
									GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You don't have enough space in your inventory that. You may be carrying to many of one of the items you are trying to purchase or you don't have enough free spaces to fit them all in your backpack!"));
									ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
									enet_peer_send(peer, 0, packet);
									delete p.data;
									break;
								}
								RemoveInventoryItem(1486, Price, peer, true);
								bool success = true;
								SaveItemMoreTimes(ItemID, count, peer, success, "Purchased from store");
								Player::PlayAudio(peer, "audio/piano_nice.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You've purchased " + to_string(count) + " `o" + getItemDef(ItemID).name + " `wfor `$" + to_string(Price) + " `wGrowtokens.\nYou have `$" + to_string(KiekTuri - Price) + " `wGrowtokens left.\n\n`5Received: ``" + to_string(count) + " " + getItemDef(ItemID).name + ""));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
							else {
								Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You can't afford `o" + getItemDef(ItemID).name + "``!  You're `$" + to_string(Price - KiekTuri) + "`` Growtokens short."));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
						}
						if (cch == "action|buy\nitem|golden_axe\n") {
							/*Fast Item Setup*/
							auto Price = 200;
							auto ItemID = 1438;
							auto count = 1;
							auto contains = false;
							auto KiekTuri = 0;
							try {
								for (auto i = 0; i < pData->inventory.items.size(); i++) {
									if (pData->inventory.items.at(i).itemID == 1486 && pData->inventory.items.at(i).itemCount >= 1) {
										KiekTuri = pData->inventory.items.at(i).itemCount;
										break;
									}
								}
							}
							catch (const std::out_of_range& e) {
								std::cout << e.what() << std::endl;
							}
							SearchInventoryItem(peer, 1486, Price, contains);
							if (contains) {
								if (CheckItemMaxed(peer, ItemID, count) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && CheckItemExists(peer, ItemID) && CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && !CheckItemExists(peer, ItemID)) {
									Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
									GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You don't have enough space in your inventory that. You may be carrying to many of one of the items you are trying to purchase or you don't have enough free spaces to fit them all in your backpack!"));
									ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
									enet_peer_send(peer, 0, packet);
									delete p.data;
									break;
								}
								RemoveInventoryItem(1486, Price, peer, true);
								bool success = true;
								SaveItemMoreTimes(ItemID, count, peer, success, "Purchased from store");
								Player::PlayAudio(peer, "audio/piano_nice.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You've purchased " + to_string(count) + " `o" + getItemDef(ItemID).name + " `wfor `$" + to_string(Price) + " `wGrowtokens.\nYou have `$" + to_string(KiekTuri - Price) + " `wGrowtokens left.\n\n`5Received: ``" + to_string(count) + " " + getItemDef(ItemID).name + ""));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
							else {
								Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You can't afford `o" + getItemDef(ItemID).name + "``!  You're `$" + to_string(Price - KiekTuri) + "`` Growtokens short."));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
						}
						if (cch == "action|buy\nitem|puddy_leash\n") {
							/*Fast Item Setup*/
							auto Price = 180;
							auto ItemID = 2032;
							auto count = 1;
							auto contains = false;
							auto KiekTuri = 0;
							try {
								for (auto i = 0; i < pData->inventory.items.size(); i++) {
									if (pData->inventory.items.at(i).itemID == 1486 && pData->inventory.items.at(i).itemCount >= 1) {
										KiekTuri = pData->inventory.items.at(i).itemCount;
										break;
									}
								}
							}
							catch (const std::out_of_range& e) {
								std::cout << e.what() << std::endl;
							}
							SearchInventoryItem(peer, 1486, Price, contains);
							if (contains) {
								if (CheckItemMaxed(peer, ItemID, count) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && CheckItemExists(peer, ItemID) && CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && !CheckItemExists(peer, ItemID)) {
									Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
									GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You don't have enough space in your inventory that. You may be carrying to many of one of the items you are trying to purchase or you don't have enough free spaces to fit them all in your backpack!"));
									ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
									enet_peer_send(peer, 0, packet);
									delete p.data;
									break;
								}
								RemoveInventoryItem(1486, Price, peer, true);
								bool success = true;
								SaveItemMoreTimes(ItemID, count, peer, success, "Purchased from store");
								Player::PlayAudio(peer, "audio/piano_nice.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You've purchased " + to_string(count) + " `o" + getItemDef(ItemID).name + " `wfor `$" + to_string(Price) + " `wGrowtokens.\nYou have `$" + to_string(KiekTuri - Price) + " `wGrowtokens left.\n\n`5Received: ``" + to_string(count) + " " + getItemDef(ItemID).name + ""));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
							else {
								Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You can't afford `o" + getItemDef(ItemID).name + "``!  You're `$" + to_string(Price - KiekTuri) + "`` Growtokens short."));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
						}
						if (cch == "action|buy\nitem|piranha\n") {
							/*Fast Item Setup*/
							auto Price = 150;
							auto ItemID = 1534;
							auto count = 1;
							auto contains = false;
							auto KiekTuri = 0;
							try {
								for (auto i = 0; i < pData->inventory.items.size(); i++) {
									if (pData->inventory.items.at(i).itemID == 1486 && pData->inventory.items.at(i).itemCount >= 1) {
										KiekTuri = pData->inventory.items.at(i).itemCount;
										break;
									}
								}
							}
							catch (const std::out_of_range& e) {
								std::cout << e.what() << std::endl;
							}
							SearchInventoryItem(peer, 1486, Price, contains);
							if (contains) {
								if (CheckItemMaxed(peer, ItemID, count) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && CheckItemExists(peer, ItemID) && CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && !CheckItemExists(peer, ItemID)) {
									Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
									GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You don't have enough space in your inventory that. You may be carrying to many of one of the items you are trying to purchase or you don't have enough free spaces to fit them all in your backpack!"));
									ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
									enet_peer_send(peer, 0, packet);
									delete p.data;
									break;
								}
								RemoveInventoryItem(1486, Price, peer, true);
								bool success = true;
								SaveItemMoreTimes(ItemID, count, peer, success, "Purchased from store");
								Player::PlayAudio(peer, "audio/piano_nice.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You've purchased " + to_string(count) + " `o" + getItemDef(ItemID).name + " `wfor `$" + to_string(Price) + " `wGrowtokens.\nYou have `$" + to_string(KiekTuri - Price) + " `wGrowtokens left.\n\n`5Received: ``" + to_string(count) + " " + getItemDef(ItemID).name + ""));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
							else {
								Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You can't afford `o" + getItemDef(ItemID).name + "``!  You're `$" + to_string(Price - KiekTuri) + "`` Growtokens short."));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
						}
						if (cch == "action|buy\nitem|muddy_pants\n") {
							/*Fast Item Setup*/
							auto Price = 125;
							auto ItemID = 2584;
							auto count = 1;
							auto contains = false;
							auto KiekTuri = 0;
							try {
								for (auto i = 0; i < pData->inventory.items.size(); i++) {
									if (pData->inventory.items.at(i).itemID == 1486 && pData->inventory.items.at(i).itemCount >= 1) {
										KiekTuri = pData->inventory.items.at(i).itemCount;
										break;
									}
								}
							}
							catch (const std::out_of_range& e) {
								std::cout << e.what() << std::endl;
							}
							SearchInventoryItem(peer, 1486, Price, contains);
							if (contains) {
								if (CheckItemMaxed(peer, ItemID, count) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && CheckItemExists(peer, ItemID) && CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && !CheckItemExists(peer, ItemID)) {
									Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
									GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You don't have enough space in your inventory that. You may be carrying to many of one of the items you are trying to purchase or you don't have enough free spaces to fit them all in your backpack!"));
									ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
									enet_peer_send(peer, 0, packet);
									delete p.data;
									break;
								}
								RemoveInventoryItem(1486, Price, peer, true);
								bool success = true;
								SaveItemMoreTimes(ItemID, count, peer, success, "Purchased from store");
								Player::PlayAudio(peer, "audio/piano_nice.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You've purchased " + to_string(count) + " `o" + getItemDef(ItemID).name + " `wfor `$" + to_string(Price) + " `wGrowtokens.\nYou have `$" + to_string(KiekTuri - Price) + " `wGrowtokens left.\n\n`5Received: ``" + to_string(count) + " " + getItemDef(ItemID).name + ""));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
							else {
								Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You can't afford `o" + getItemDef(ItemID).name + "``!  You're `$" + to_string(Price - KiekTuri) + "`` Growtokens short."));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
						}
						if (cch == "action|buy\nitem|night_vision\n") {
							/*Fast Item Setup*/
							auto Price = 110;
							auto ItemID = 3576;
							auto count = 1;
							auto contains = false;
							auto KiekTuri = 0;
							try {
								for (auto i = 0; i < pData->inventory.items.size(); i++) {
									if (pData->inventory.items.at(i).itemID == 1486 && pData->inventory.items.at(i).itemCount >= 1) {
										KiekTuri = pData->inventory.items.at(i).itemCount;
										break;
									}
								}
							}
							catch (const std::out_of_range& e) {
								std::cout << e.what() << std::endl;
							}
							SearchInventoryItem(peer, 1486, Price, contains);
							if (contains) {
								if (CheckItemMaxed(peer, ItemID, count) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && CheckItemExists(peer, ItemID) && CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && !CheckItemExists(peer, ItemID)) {
									Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
									GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You don't have enough space in your inventory that. You may be carrying to many of one of the items you are trying to purchase or you don't have enough free spaces to fit them all in your backpack!"));
									ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
									enet_peer_send(peer, 0, packet);
									delete p.data;
									break;
								}
								RemoveInventoryItem(1486, Price, peer, true);
								bool success = true;
								SaveItemMoreTimes(ItemID, count, peer, success, "Purchased from store");
								Player::PlayAudio(peer, "audio/piano_nice.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You've purchased " + to_string(count) + " `o" + getItemDef(ItemID).name + " `wfor `$" + to_string(Price) + " `wGrowtokens.\nYou have `$" + to_string(KiekTuri - Price) + " `wGrowtokens left.\n\n`5Received: ``" + to_string(count) + " " + getItemDef(ItemID).name + ""));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
							else {
								Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You can't afford `o" + getItemDef(ItemID).name + "``!  You're `$" + to_string(Price - KiekTuri) + "`` Growtokens short."));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
						}
						if (cch == "action|buy\nitem|grip_tape\n") {
							/*Fast Item Setup*/
							auto Price = 100;
							auto ItemID = 3248;
							auto count = 1;
							auto contains = false;
							auto KiekTuri = 0;
							try {
								for (auto i = 0; i < pData->inventory.items.size(); i++) {
									if (pData->inventory.items.at(i).itemID == 1486 && pData->inventory.items.at(i).itemCount >= 1) {
										KiekTuri = pData->inventory.items.at(i).itemCount;
										break;
									}
								}
							}
							catch (const std::out_of_range& e) {
								std::cout << e.what() << std::endl;
							}
							SearchInventoryItem(peer, 1486, Price, contains);
							if (contains) {
								if (CheckItemMaxed(peer, ItemID, count) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && CheckItemExists(peer, ItemID) && CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && !CheckItemExists(peer, ItemID)) {
									Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
									GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You don't have enough space in your inventory that. You may be carrying to many of one of the items you are trying to purchase or you don't have enough free spaces to fit them all in your backpack!"));
									ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
									enet_peer_send(peer, 0, packet);
									delete p.data;
									break;
								}
								RemoveInventoryItem(1486, Price, peer, true);
								bool success = true;
								SaveItemMoreTimes(ItemID, count, peer, success, "Purchased from store");
								Player::PlayAudio(peer, "audio/piano_nice.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You've purchased " + to_string(count) + " `o" + getItemDef(ItemID).name + " `wfor `$" + to_string(Price) + " `wGrowtokens.\nYou have `$" + to_string(KiekTuri - Price) + " `wGrowtokens left.\n\n`5Received: ``" + to_string(count) + " " + getItemDef(ItemID).name + ""));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
							else {
								Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You can't afford `o" + getItemDef(ItemID).name + "``!  You're `$" + to_string(Price - KiekTuri) + "`` Growtokens short."));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
						}
						if (cch == "action|buy\nitem|doodad\n") {
							/*Fast Item Setup*/
							auto Price = 75;
							auto ItemID = 1492;
							auto count = 1;
							auto contains = false;
							auto KiekTuri = 0;
							try {
								for (auto i = 0; i < pData->inventory.items.size(); i++) {
									if (pData->inventory.items.at(i).itemID == 1486 && pData->inventory.items.at(i).itemCount >= 1) {
										KiekTuri = pData->inventory.items.at(i).itemCount;
										break;
									}
								}
							}
							catch (const std::out_of_range& e) {
								std::cout << e.what() << std::endl;
							}
							SearchInventoryItem(peer, 1486, Price, contains);
							if (contains) {
								if (CheckItemMaxed(peer, ItemID, count) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && CheckItemExists(peer, ItemID) && CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && !CheckItemExists(peer, ItemID)) {
									Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
									GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You don't have enough space in your inventory that. You may be carrying to many of one of the items you are trying to purchase or you don't have enough free spaces to fit them all in your backpack!"));
									ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
									enet_peer_send(peer, 0, packet);
									delete p.data;
									break;
								}
								RemoveInventoryItem(1486, Price, peer, true);
								bool success = true;
								SaveItemMoreTimes(ItemID, count, peer, success, "Purchased from store");
								Player::PlayAudio(peer, "audio/piano_nice.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You've purchased " + to_string(count) + " `o" + getItemDef(ItemID).name + " `wfor `$" + to_string(Price) + " `wGrowtokens.\nYou have `$" + to_string(KiekTuri - Price) + " `wGrowtokens left.\n\n`5Received: ``" + to_string(count) + " " + getItemDef(ItemID).name + ""));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
							else {
								Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You can't afford `o" + getItemDef(ItemID).name + "``!  You're `$" + to_string(Price - KiekTuri) + "`` Growtokens short."));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
						}
						if (cch == "action|buy\nitem|spike_juice\n") {
							/*Fast Item Setup*/
							auto Price = 60;
							auto ItemID = 1662;
							auto count = 1;
							auto contains = false;
							auto KiekTuri = 0;
							try {
								for (auto i = 0; i < pData->inventory.items.size(); i++) {
									if (pData->inventory.items.at(i).itemID == 1486 && pData->inventory.items.at(i).itemCount >= 1) {
										KiekTuri = pData->inventory.items.at(i).itemCount;
										break;
									}
								}
							}
							catch (const std::out_of_range& e) {
								std::cout << e.what() << std::endl;
							}
							SearchInventoryItem(peer, 1486, Price, contains);
							if (contains) {
								if (CheckItemMaxed(peer, ItemID, count) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && CheckItemExists(peer, ItemID) && CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && !CheckItemExists(peer, ItemID)) {
									Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
									GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You don't have enough space in your inventory that. You may be carrying to many of one of the items you are trying to purchase or you don't have enough free spaces to fit them all in your backpack!"));
									ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
									enet_peer_send(peer, 0, packet);
									delete p.data;
									break;
								}
								RemoveInventoryItem(1486, Price, peer, true);
								bool success = true;
								SaveItemMoreTimes(ItemID, count, peer, success, "Purchased from store");
								Player::PlayAudio(peer, "audio/piano_nice.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You've purchased " + to_string(count) + " `o" + getItemDef(ItemID).name + " `wfor `$" + to_string(Price) + " `wGrowtokens.\nYou have `$" + to_string(KiekTuri - Price) + " `wGrowtokens left.\n\n`5Received: ``" + to_string(count) + " " + getItemDef(ItemID).name + ""));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
							else {
								Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You can't afford `o" + getItemDef(ItemID).name + "``!  You're `$" + to_string(Price - KiekTuri) + "`` Growtokens short."));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
						}
						if (cch == "action|buy\nitem|nothingness\n") {
							/*Fast Item Setup*/
							auto Price = 50;
							auto ItemID = 1490;
							auto count = 1;
							auto contains = false;
							auto KiekTuri = 0;
							try {
								for (auto i = 0; i < pData->inventory.items.size(); i++) {
									if (pData->inventory.items.at(i).itemID == 1486 && pData->inventory.items.at(i).itemCount >= 1) {
										KiekTuri = pData->inventory.items.at(i).itemCount;
										break;
									}
								}
							}
							catch (const std::out_of_range& e) {
								std::cout << e.what() << std::endl;
							}
							SearchInventoryItem(peer, 1486, Price, contains);
							if (contains) {
								if (CheckItemMaxed(peer, ItemID, count) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && CheckItemExists(peer, ItemID) && CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && !CheckItemExists(peer, ItemID)) {
									Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
									GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You don't have enough space in your inventory that. You may be carrying to many of one of the items you are trying to purchase or you don't have enough free spaces to fit them all in your backpack!"));
									ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
									enet_peer_send(peer, 0, packet);
									delete p.data;
									break;
								}
								RemoveInventoryItem(1486, Price, peer, true);
								bool success = true;
								SaveItemMoreTimes(ItemID, count, peer, success, "Purchased from store");
								Player::PlayAudio(peer, "audio/piano_nice.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You've purchased " + to_string(count) + " `o" + getItemDef(ItemID).name + " `wfor `$" + to_string(Price) + " `wGrowtokens.\nYou have `$" + to_string(KiekTuri - Price) + " `wGrowtokens left.\n\n`5Received: ``" + to_string(count) + " " + getItemDef(ItemID).name + ""));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
							else {
								Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You can't afford `o" + getItemDef(ItemID).name + "``!  You're `$" + to_string(Price - KiekTuri) + "`` Growtokens short."));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
						}
						if (cch == "action|buy\nitem|dirt_gun\n") {
							/*Fast Item Setup*/
							auto Price = 40;
							auto ItemID = 2876;
							auto count = 1;
							auto contains = false;
							auto KiekTuri = 0;
							try {
								for (auto i = 0; i < pData->inventory.items.size(); i++) {
									if (pData->inventory.items.at(i).itemID == 1486 && pData->inventory.items.at(i).itemCount >= 1) {
										KiekTuri = pData->inventory.items.at(i).itemCount;
										break;
									}
								}
							}
							catch (const std::out_of_range& e) {
								std::cout << e.what() << std::endl;
							}
							SearchInventoryItem(peer, 1486, Price, contains);
							if (contains) {
								if (CheckItemMaxed(peer, ItemID, count) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && CheckItemExists(peer, ItemID) && CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && !CheckItemExists(peer, ItemID)) {
									Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
									GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You don't have enough space in your inventory that. You may be carrying to many of one of the items you are trying to purchase or you don't have enough free spaces to fit them all in your backpack!"));
									ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
									enet_peer_send(peer, 0, packet);
									delete p.data;
									break;
								}
								RemoveInventoryItem(1486, Price, peer, true);
								bool success = true;
								SaveItemMoreTimes(ItemID, count, peer, success, "Purchased from store");
								Player::PlayAudio(peer, "audio/piano_nice.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You've purchased " + to_string(count) + " `o" + getItemDef(ItemID).name + " `wfor `$" + to_string(Price) + " `wGrowtokens.\nYou have `$" + to_string(KiekTuri - Price) + " `wGrowtokens left.\n\n`5Received: ``" + to_string(count) + " " + getItemDef(ItemID).name + ""));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
							else {
								Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You can't afford `o" + getItemDef(ItemID).name + "``!  You're `$" + to_string(Price - KiekTuri) + "`` Growtokens short."));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
						}
						if (cch == "action|buy\nitem|derpy_star\n") {
							/*Fast Item Setup*/
							auto Price = 30;
							auto ItemID = 1628;
							auto count = 1;
							auto contains = false;
							auto KiekTuri = 0;
							try {
								for (auto i = 0; i < pData->inventory.items.size(); i++) {
									if (pData->inventory.items.at(i).itemID == 1486 && pData->inventory.items.at(i).itemCount >= 1) {
										KiekTuri = pData->inventory.items.at(i).itemCount;
										break;
									}
								}
							}
							catch (const std::out_of_range& e) {
								std::cout << e.what() << std::endl;
							}
							SearchInventoryItem(peer, 1486, Price, contains);
							if (contains) {
								if (CheckItemMaxed(peer, ItemID, count) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && CheckItemExists(peer, ItemID) && CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && !CheckItemExists(peer, ItemID)) {
									Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
									GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You don't have enough space in your inventory that. You may be carrying to many of one of the items you are trying to purchase or you don't have enough free spaces to fit them all in your backpack!"));
									ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
									enet_peer_send(peer, 0, packet);
									delete p.data;
									break;
								}
								RemoveInventoryItem(1486, Price, peer, true);
								bool success = true;
								SaveItemMoreTimes(ItemID, count, peer, success, "Purchased from store");
								Player::PlayAudio(peer, "audio/piano_nice.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You've purchased " + to_string(count) + " `o" + getItemDef(ItemID).name + " `wfor `$" + to_string(Price) + " `wGrowtokens.\nYou have `$" + to_string(KiekTuri - Price) + " `wGrowtokens left.\n\n`5Received: ``" + to_string(count) + " " + getItemDef(ItemID).name + ""));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
							else {
								Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You can't afford `o" + getItemDef(ItemID).name + "``!  You're `$" + to_string(Price - KiekTuri) + "`` Growtokens short."));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
						}
						if (cch == "action|buy\nitem|mini_mod\n") {
							/*Fast Item Setup*/
							auto Price = 20;
							auto ItemID = 4758;
							auto count = 1;
							auto contains = false;
							auto KiekTuri = 0;
							try {
								for (auto i = 0; i < pData->inventory.items.size(); i++) {
									if (pData->inventory.items.at(i).itemID == 1486 && pData->inventory.items.at(i).itemCount >= 1) {
										KiekTuri = pData->inventory.items.at(i).itemCount;
										break;
									}
								}
							}
							catch (const std::out_of_range& e) {
								std::cout << e.what() << std::endl;
							}
							SearchInventoryItem(peer, 1486, Price, contains);
							if (contains) {
								if (CheckItemMaxed(peer, ItemID, count) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && CheckItemExists(peer, ItemID) && CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && !CheckItemExists(peer, ItemID)) {
									Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
									GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You don't have enough space in your inventory that. You may be carrying to many of one of the items you are trying to purchase or you don't have enough free spaces to fit them all in your backpack!"));
									ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
									enet_peer_send(peer, 0, packet);
									delete p.data;
									break;
								}
								RemoveInventoryItem(1486, Price, peer, true);
								bool success = true;
								SaveItemMoreTimes(ItemID, count, peer, success, "Purchased from store");
								Player::PlayAudio(peer, "audio/piano_nice.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You've purchased " + to_string(count) + " `o" + getItemDef(ItemID).name + " `wfor `$" + to_string(Price) + " `wGrowtokens.\nYou have `$" + to_string(KiekTuri - Price) + " `wGrowtokens left.\n\n`5Received: ``" + to_string(count) + " " + getItemDef(ItemID).name + ""));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
							else {
								Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You can't afford `o" + getItemDef(ItemID).name + "``!  You're `$" + to_string(Price - KiekTuri) + "`` Growtokens short."));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
						}
						if (cch == "action|buy\nitem|growmoji_pack\n") {
							/*Fast Item Setup*/
							vector<int> itemids{ 5770, 5772, 5774, 5776, 5778 };
							auto Price = 15;
							auto ItemID = itemids[rand() % itemids.size()];
							auto count = 1;
							auto contains = false;
							auto KiekTuri = 0;
							try {
								for (auto i = 0; i < pData->inventory.items.size(); i++) {
									if (pData->inventory.items.at(i).itemID == 1486 && pData->inventory.items.at(i).itemCount >= 1) {
										KiekTuri = pData->inventory.items.at(i).itemCount;
										break;
									}
								}
							}
							catch (const std::out_of_range& e) {
								std::cout << e.what() << std::endl;
							}
							SearchInventoryItem(peer, 1486, Price, contains);
							if (contains) {
								if (CheckItemMaxed(peer, ItemID, count) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && CheckItemExists(peer, ItemID) && CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && !CheckItemExists(peer, ItemID)) {
									Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
									GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You don't have enough space in your inventory that. You may be carrying to many of one of the items you are trying to purchase or you don't have enough free spaces to fit them all in your backpack!"));
									ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
									enet_peer_send(peer, 0, packet);
									delete p.data;
									break;
								}
								RemoveInventoryItem(1486, Price, peer, true);
								bool success = true;
								SaveItemMoreTimes(ItemID, count, peer, success, "Purchased from store");
								Player::PlayAudio(peer, "audio/piano_nice.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You've purchased " + to_string(count) + " `oGrowmoji Mystery Box `wfor `$" + to_string(Price) + " `wGrowtokens.\nYou have `$" + to_string(KiekTuri - Price) + " `wGrowtokens left.\n\n`5Received: ``" + to_string(count) + " " + getItemDef(ItemID).name + ""));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
							else {
								Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You can't afford `oGrowmoji Mystery Box``!  You're `$" + to_string(Price - KiekTuri) + "`` Growtokens short."));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
						}
						if (cch == "action|buy\nitem|megaphone\n") {
							/*Fast Item Setup*/
							auto Price = 10;
							auto ItemID = 2480;
							auto count = 1;
							auto contains = false;
							auto KiekTuri = 0;
							try {
								for (auto i = 0; i < pData->inventory.items.size(); i++) {
									if (pData->inventory.items.at(i).itemID == 1486 && pData->inventory.items.at(i).itemCount >= 1) {
										KiekTuri = pData->inventory.items.at(i).itemCount;
										break;
									}
								}
							}
							catch (const std::out_of_range& e) {
								std::cout << e.what() << std::endl;
							}
							SearchInventoryItem(peer, 1486, Price, contains);
							if (contains) {
								if (CheckItemMaxed(peer, ItemID, count) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && CheckItemExists(peer, ItemID) && CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && !CheckItemExists(peer, ItemID)) {
									Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
									GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You don't have enough space in your inventory that. You may be carrying to many of one of the items you are trying to purchase or you don't have enough free spaces to fit them all in your backpack!"));
									ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
									enet_peer_send(peer, 0, packet);
									delete p.data;
									break;
								}
								RemoveInventoryItem(1486, Price, peer, true);
								bool success = true;
								SaveItemMoreTimes(ItemID, count, peer, success, "Purchased from store");
								Player::PlayAudio(peer, "audio/piano_nice.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You've purchased " + to_string(count) + " `o" + getItemDef(ItemID).name + " `wfor `$" + to_string(Price) + " `wGrowtokens.\nYou have `$" + to_string(KiekTuri - Price) + " `wGrowtokens left.\n\n`5Received: ``" + to_string(count) + " " + getItemDef(ItemID).name + ""));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
							else {
								Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You can't afford `o" + getItemDef(ItemID).name + "``!  You're `$" + to_string(Price - KiekTuri) + "`` Growtokens short."));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
						}
						if (cch == "action|buy\nitem|xp_potion\n") {
							/*Fast Item Setup*/
							auto Price = 10;
							auto ItemID = 1488;
							auto count = 1;
							auto contains = false;
							auto KiekTuri = 0;
							try {
								for (auto i = 0; i < pData->inventory.items.size(); i++) {
									if (pData->inventory.items.at(i).itemID == 1486 && pData->inventory.items.at(i).itemCount >= 1) {
										KiekTuri = pData->inventory.items.at(i).itemCount;
										break;
									}
								}
							}
							catch (const std::out_of_range& e) {
								std::cout << e.what() << std::endl;
							}
							SearchInventoryItem(peer, 1486, Price, contains);
							if (contains) {
								if (CheckItemMaxed(peer, ItemID, count) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && CheckItemExists(peer, ItemID) && CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && !CheckItemExists(peer, ItemID)) {
									Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
									GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You don't have enough space in your inventory that. You may be carrying to many of one of the items you are trying to purchase or you don't have enough free spaces to fit them all in your backpack!"));
									ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
									enet_peer_send(peer, 0, packet);
									delete p.data;
									break;
								}
								RemoveInventoryItem(1486, Price, peer, true);
								bool success = true;
								SaveItemMoreTimes(ItemID, count, peer, success, "Purchased from store");
								Player::PlayAudio(peer, "audio/piano_nice.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You've purchased " + to_string(count) + " `o" + getItemDef(ItemID).name + " `wfor `$" + to_string(Price) + " `wGrowtokens.\nYou have `$" + to_string(KiekTuri - Price) + " `wGrowtokens left.\n\n`5Received: ``" + to_string(count) + " " + getItemDef(ItemID).name + ""));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
							else {
								Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You can't afford `o" + getItemDef(ItemID).name + "``!  You're `$" + to_string(Price - KiekTuri) + "`` Growtokens short."));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
						}
						if (cch == "action|buy\nitem|challenge_timer\n") {
							/*Fast Item Setup*/
							auto Price = 5;
							auto ItemID = 3804;
							auto count = 1;
							auto contains = false;
							auto KiekTuri = 0;
							try {
								for (auto i = 0; i < pData->inventory.items.size(); i++) {
									if (pData->inventory.items.at(i).itemID == 1486 && pData->inventory.items.at(i).itemCount >= 1) {
										KiekTuri = pData->inventory.items.at(i).itemCount;
										break;
									}
								}
							}
							catch (const std::out_of_range& e) {
								std::cout << e.what() << std::endl;
							}
							SearchInventoryItem(peer, 1486, Price, contains);
							if (contains) {
								if (CheckItemMaxed(peer, ItemID, count) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && CheckItemExists(peer, ItemID) && CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && !CheckItemExists(peer, ItemID)) {
									Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
									GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You don't have enough space in your inventory that. You may be carrying to many of one of the items you are trying to purchase or you don't have enough free spaces to fit them all in your backpack!"));
									ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
									enet_peer_send(peer, 0, packet);
									delete p.data;
									break;
								}
								RemoveInventoryItem(1486, Price, peer, true);
								bool success = true;
								SaveItemMoreTimes(ItemID, count, peer, success, "Purchased from store");
								Player::PlayAudio(peer, "audio/piano_nice.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You've purchased " + to_string(count) + " `o" + getItemDef(ItemID).name + " `wfor `$" + to_string(Price) + " `wGrowtokens.\nYou have `$" + to_string(KiekTuri - Price) + " `wGrowtokens left.\n\n`5Received: ``" + to_string(count) + " " + getItemDef(ItemID).name + ""));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
							else {
								Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You can't afford `o" + getItemDef(ItemID).name + "``!  You're `$" + to_string(Price - KiekTuri) + "`` Growtokens short."));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
						}
						if (cch == "action|buy\nitem|focused_eyes\n") {
							/*Fast Item Setup*/
							auto Price = 100;
							auto ItemID = 1204;
							auto count = 1;
							auto contains = false;
							auto KiekTuri = 0;
							try {
								for (auto i = 0; i < pData->inventory.items.size(); i++) {
									if (pData->inventory.items.at(i).itemID == 1486 && pData->inventory.items.at(i).itemCount >= 1) {
										KiekTuri = pData->inventory.items.at(i).itemCount;
										break;
									}
								}
							}
							catch (const std::out_of_range& e) {
								std::cout << e.what() << std::endl;
							}
							SearchInventoryItem(peer, 1486, Price, contains);
							if (contains) {
								if (CheckItemMaxed(peer, ItemID, count) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && CheckItemExists(peer, ItemID) && CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && !CheckItemExists(peer, ItemID)) {
									Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
									GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You don't have enough space in your inventory that. You may be carrying to many of one of the items you are trying to purchase or you don't have enough free spaces to fit them all in your backpack!"));
									ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
									enet_peer_send(peer, 0, packet);
									delete p.data;
									break;
								}
								RemoveInventoryItem(1486, Price, peer, true);
								bool success = true;
								SaveItemMoreTimes(ItemID, count, peer, success, "Purchased from store");
								Player::PlayAudio(peer, "audio/piano_nice.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You've purchased " + to_string(count) + " `o" + getItemDef(ItemID).name + " `wfor `$" + to_string(Price) + " `wGrowtokens.\nYou have `$" + to_string(KiekTuri - Price) + " `wGrowtokens left.\n\n`5Received: ``" + to_string(count) + " " + getItemDef(ItemID).name + ""));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
							else {
								Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You can't afford `o" + getItemDef(ItemID).name + "``!  You're `$" + to_string(Price - KiekTuri) + "`` Growtokens short."));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
						}
						if (cch == "action|buy\nitem|crystal_cape\n") {
							/*Fast Item Setup*/
							auto Price = 90;
							auto ItemID = 1738;
							auto count = 1;
							auto contains = false;
							auto KiekTuri = 0;
							try {
								for (auto i = 0; i < pData->inventory.items.size(); i++) {
									if (pData->inventory.items.at(i).itemID == 1486 && pData->inventory.items.at(i).itemCount >= 1) {
										KiekTuri = pData->inventory.items.at(i).itemCount;
										break;
									}
								}
							}
							catch (const std::out_of_range& e) {
								std::cout << e.what() << std::endl;
							}
							SearchInventoryItem(peer, 1486, Price, contains);
							if (contains) {
								if (CheckItemMaxed(peer, ItemID, count) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && CheckItemExists(peer, ItemID) && CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && !CheckItemExists(peer, ItemID)) {
									Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
									GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You don't have enough space in your inventory that. You may be carrying to many of one of the items you are trying to purchase or you don't have enough free spaces to fit them all in your backpack!"));
									ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
									enet_peer_send(peer, 0, packet);
									delete p.data;
									break;
								}
								RemoveInventoryItem(1486, Price, peer, true);
								bool success = true;
								SaveItemMoreTimes(ItemID, count, peer, success, "Purchased from store");
								Player::PlayAudio(peer, "audio/piano_nice.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You've purchased " + to_string(count) + " `o" + getItemDef(ItemID).name + " `wfor `$" + to_string(Price) + " `wGrowtokens.\nYou have `$" + to_string(KiekTuri - Price) + " `wGrowtokens left.\n\n`5Received: ``" + to_string(count) + " " + getItemDef(ItemID).name + ""));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
							else {
								Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You can't afford `o" + getItemDef(ItemID).name + "``!  You're `$" + to_string(Price - KiekTuri) + "`` Growtokens short."));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
						}
						if (cch == "action|buy\nitem|diggers_spade\n") {
							/*Fast Item Setup*/
							auto Price = 200;
							auto ItemID = 2952;
							auto count = 1;
							auto contains = false;
							auto KiekTuri = 0;
							try {
								for (auto i = 0; i < pData->inventory.items.size(); i++) {
									if (pData->inventory.items.at(i).itemID == 1486 && pData->inventory.items.at(i).itemCount >= 1) {
										KiekTuri = pData->inventory.items.at(i).itemCount;
										break;
									}
								}
							}
							catch (const std::out_of_range& e) {
								std::cout << e.what() << std::endl;
							}
							SearchInventoryItem(peer, 1486, Price, contains);
							if (contains) {
								if (CheckItemMaxed(peer, ItemID, count) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && CheckItemExists(peer, ItemID) && CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && !CheckItemExists(peer, ItemID)) {
									Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
									GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You don't have enough space in your inventory that. You may be carrying to many of one of the items you are trying to purchase or you don't have enough free spaces to fit them all in your backpack!"));
									ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
									enet_peer_send(peer, 0, packet);
									delete p.data;
									break;
								}
								RemoveInventoryItem(1486, Price, peer, true);
								bool success = true;
								SaveItemMoreTimes(ItemID, count, peer, success, "Purchased from store");
								Player::PlayAudio(peer, "audio/piano_nice.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You've purchased " + to_string(count) + " `o" + getItemDef(ItemID).name + " `wfor `$" + to_string(Price) + " `wGrowtokens.\nYou have `$" + to_string(KiekTuri - Price) + " `wGrowtokens left.\n\n`5Received: ``" + to_string(count) + " " + getItemDef(ItemID).name + ""));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
							else {
								Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You can't afford `o" + getItemDef(ItemID).name + "``!  You're `$" + to_string(Price - KiekTuri) + "`` Growtokens short."));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
						}
						if (cch == "action|buy\nitem|rare_seed\n") {
							ifstream ifsz("save/gemdb/_" + pData->rawName + ".txt");
							string content((std::istreambuf_iterator<char>(ifsz)), (std::istreambuf_iterator<char>()));
							auto gembux = atoi(content.c_str());
							if (gembux >= 1000) {
								int Seed1 = 0;
								int Seed2 = 0;
								int Seed3 = 0;
								int Seed4 = 0;
								int Seed5 = 0;
								int AVGRarity = rand() % 10 + 10;
								while (Seed1 == 0 || Seed2 == 0 || Seed3 == 0 || Seed4 == 0 || Seed5 == 0) {
									for (int i = 0; i < maxItems; i++) {
										if (i >= 1000) {
											Player::OnTalkBubble(peer, pData->netID, "Something went wrong.", 0, true);
											break;
										}
										if (isSeed(i) && getItemDef(i).rarity == AVGRarity || isSeed(i) && getItemDef(i).rarity == AVGRarity + 1) {
											if (Seed1 == 0) Seed1 = i;
											else if (Seed2 == 0) Seed2 = i;
											else if (Seed3 == 0) Seed3 = i;
											else if (Seed4 == 0) Seed4 = i;
											else if (Seed5 == 0) Seed5 = i;
											else break;
											AVGRarity = rand() % 10 + 10;
										}
										else if (i == maxItems - 1) {
											break;
										}
									}
								}
								if (CheckItemMaxed(peer, Seed1, 1) || CheckItemMaxed(peer, Seed2, 1) || CheckItemMaxed(peer, Seed3, 1) || CheckItemMaxed(peer, Seed4, 75) || CheckItemMaxed(peer, Seed5, 1) || pData->inventory.items.size() + 5 >= pData->currentInventorySize) {
									Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
									GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You don't have enough space in your inventory that. You may be carrying to many of one of the items you are trying to purchase or you don't have enough free spaces to fit them all in your backpack!"));
									ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
									enet_peer_send(peer, 0, packet);
									delete p.data;
									break;
								}
								gembux -= 1000;
								ofstream myfile;
								myfile.open("save/gemdb/_" + pData->rawName + ".txt");
								myfile << gembux;
								myfile.close();
								Player::OnSetBux(peer, gembux, 0);
								bool success = true;
								SaveItemMoreTimes(Seed1, 1, peer, success, "Purchased from store");
								SaveItemMoreTimes(Seed2, 1, peer, success, "Purchased from store");
								SaveItemMoreTimes(Seed3, 1, peer, success, "Purchased from store");
								SaveItemMoreTimes(Seed4, 1, peer, success, "Purchased from store");
								SaveItemMoreTimes(Seed5, 1, peer, success, "Purchased from store");
								Player::PlayAudio(peer, "audio/piano_nice.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You've purchased `oRare Seed Pack `wfor `$" + to_string(1000) + " `wGems.\nYou have `$" + to_string(gembux) + " `wGems left.\n\n`5Received:`` " + getItemDef(Seed1).name + "`$,`` " + getItemDef(Seed2).name + "`$,`` " + getItemDef(Seed3).name + "`$,`` " + getItemDef(Seed4).name + "`$,`` " + getItemDef(Seed5).name + "\n"));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
							else {
								Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You can't afford `oRare Seed Pack``!  You're `$" + to_string(1000 - gembux) + "`` Gems short."));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
						}
						if (cch == "action|buy\nitem|geiger\n") {
							/*Fast Item Setup*/
							auto Price = 25000;
							auto ItemID = 2204;
							auto count = 1;
							ifstream ifsz("save/gemdb/_" + pData->rawName + ".txt");
							string content((std::istreambuf_iterator<char>(ifsz)), (std::istreambuf_iterator<char>()));
							auto gembux = atoi(content.c_str());
							if (gembux >= Price) {
								if (CheckItemMaxed(peer, ItemID, count) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && CheckItemExists(peer, ItemID) && CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && !CheckItemExists(peer, ItemID)) {
									Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
									GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You don't have enough space in your inventory that. You may be carrying to many of one of the items you are trying to purchase or you don't have enough free spaces to fit them all in your backpack!"));
									ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
									enet_peer_send(peer, 0, packet);
									delete p.data;
									break;
								}
								gembux -= Price;
								ofstream myfile;
								myfile.open("save/gemdb/_" + pData->rawName + ".txt");
								myfile << gembux;
								myfile.close();
								Player::OnSetBux(peer, gembux, 0);
								bool success = true;
								SaveItemMoreTimes(ItemID, count, peer, success, "Purchased from store");
								Player::PlayAudio(peer, "audio/piano_nice.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You've purchased " + to_string(count) + " `o" + getItemDef(ItemID).name + " `wfor `$" + to_string(Price) + " `wGems.\nYou have `$" + to_string(gembux) + " `wGems left.\n\n`5Received: ``" + to_string(count) + " " + getItemDef(ItemID).name + ""));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
							else {
								Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You can't afford `o" + getItemDef(ItemID).name + "``!  You're `$" + to_string(Price - gembux) + "`` Gems short."));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
						}
						if (cch == "action|buy\nitem|guild_name_changer\n") {
							/*Fast Item Setup*/
							auto Price = 100000;
							auto ItemID = 7190;
							auto count = 1;
							ifstream ifsz("save/gemdb/_" + pData->rawName + ".txt");
							string content((std::istreambuf_iterator<char>(ifsz)), (std::istreambuf_iterator<char>()));
							auto gembux = atoi(content.c_str());
							if (gembux >= Price) {
								if (CheckItemMaxed(peer, ItemID, count) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && CheckItemExists(peer, ItemID) && CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && !CheckItemExists(peer, ItemID)) {
									Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
									GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You don't have enough space in your inventory that. You may be carrying to many of one of the items you are trying to purchase or you don't have enough free spaces to fit them all in your backpack!"));
									ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
									enet_peer_send(peer, 0, packet);
									delete p.data;
									break;
								}
								gembux -= Price;
								ofstream myfile;
								myfile.open("save/gemdb/_" + pData->rawName + ".txt");
								myfile << gembux;
								myfile.close();
								Player::OnSetBux(peer, gembux, 0);
								bool success = true;
								SaveItemMoreTimes(ItemID, count, peer, success, "Purchased from store");
								Player::PlayAudio(peer, "audio/piano_nice.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You've purchased " + to_string(count) + " `o" + getItemDef(ItemID).name + " `wfor `$" + to_string(Price) + " `wGems.\nYou have `$" + to_string(gembux) + " `wGems left.\n\n`5Received: ``" + to_string(count) + " " + getItemDef(ItemID).name + ""));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
							else {
								Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You can't afford `o" + getItemDef(ItemID).name + "``!  You're `$" + to_string(Price - gembux) + "`` Gems short."));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
						}
						if (cch == "action|buy\nitem|ssp_10_pack\n") {
							/*Fast Item Setup*/
							auto Price = 1000;
							auto ItemID = 5706;
							auto count = 10;
							ifstream ifsz("save/gemdb/_" + pData->rawName + ".txt");
							string content((std::istreambuf_iterator<char>(ifsz)), (std::istreambuf_iterator<char>()));
							auto gembux = atoi(content.c_str());
							if (gembux >= Price) {
								if (CheckItemMaxed(peer, ItemID, count) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && CheckItemExists(peer, ItemID) && CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && !CheckItemExists(peer, ItemID)) {
									Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
									GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You don't have enough space in your inventory that. You may be carrying to many of one of the items you are trying to purchase or you don't have enough free spaces to fit them all in your backpack!"));
									ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
									enet_peer_send(peer, 0, packet);
									delete p.data;
									break;
								}
								gembux -= Price;
								ofstream myfile;
								myfile.open("save/gemdb/_" + pData->rawName + ".txt");
								myfile << gembux;
								myfile.close();
								Player::OnSetBux(peer, gembux, 0);
								bool success = true;
								SaveItemMoreTimes(ItemID, count, peer, success, "Purchased from store");
								Player::PlayAudio(peer, "audio/piano_nice.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You've purchased " + to_string(count) + " `o" + getItemDef(ItemID).name + " `wfor `$" + to_string(Price) + " `wGems.\nYou have `$" + to_string(gembux) + " `wGems left.\n\n`5Received: ``" + to_string(count) + " " + getItemDef(ItemID).name + ""));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
							else {
								Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You can't afford `o" + getItemDef(ItemID).name + "``!  You're `$" + to_string(Price - gembux) + "`` Gems short."));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
						}
						if (cch == "action|buy\nitem|guild_chest_pack\n") {
							/*Fast Item Setup*/
							auto Price = 20000;
							auto ItemID = 5954;
							auto count = 10;
							ifstream ifsz("save/gemdb/_" + pData->rawName + ".txt");
							string content((std::istreambuf_iterator<char>(ifsz)), (std::istreambuf_iterator<char>()));
							auto gembux = atoi(content.c_str());
							if (gembux >= Price) {
								if (CheckItemMaxed(peer, ItemID, count) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && CheckItemExists(peer, ItemID) && CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && !CheckItemExists(peer, ItemID)) {
									Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
									GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You don't have enough space in your inventory that. You may be carrying to many of one of the items you are trying to purchase or you don't have enough free spaces to fit them all in your backpack!"));
									ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
									enet_peer_send(peer, 0, packet);
									delete p.data;
									break;
								}
								gembux -= Price;
								ofstream myfile;
								myfile.open("save/gemdb/_" + pData->rawName + ".txt");
								myfile << gembux;
								myfile.close();
								Player::OnSetBux(peer, gembux, 0);
								bool success = true;
								SaveItemMoreTimes(ItemID, count, peer, success, "Purchased from store");
								Player::PlayAudio(peer, "audio/piano_nice.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You've purchased " + to_string(count) + " `o" + getItemDef(ItemID).name + " `wfor `$" + to_string(Price) + " `wGems.\nYou have `$" + to_string(gembux) + " `wGems left.\n\n`5Received: ``" + to_string(count) + " " + getItemDef(ItemID).name + ""));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
							else {
								Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You can't afford `o" + getItemDef(ItemID).name + "``!  You're `$" + to_string(Price - gembux) + "`` Gems short."));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
						}
						if (cch == "action|buy\nitem|bountiful_seed_pack\n") {
							/*Fast Item Setup*/
							auto Price = 1000;
							auto ItemID = 8970;
							auto count = 1;
							ifstream ifsz("save/gemdb/_" + pData->rawName + ".txt");
							string content((std::istreambuf_iterator<char>(ifsz)), (std::istreambuf_iterator<char>()));
							auto gembux = atoi(content.c_str());
							if (gembux >= Price) {
								if (CheckItemMaxed(peer, ItemID, count) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && CheckItemExists(peer, ItemID) && CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && !CheckItemExists(peer, ItemID)) {
									Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
									GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You don't have enough space in your inventory that. You may be carrying to many of one of the items you are trying to purchase or you don't have enough free spaces to fit them all in your backpack!"));
									ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
									enet_peer_send(peer, 0, packet);
									delete p.data;
									break;
								}
								gembux -= Price;
								ofstream myfile;
								myfile.open("save/gemdb/_" + pData->rawName + ".txt");
								myfile << gembux;
								myfile.close();
								Player::OnSetBux(peer, gembux, 0);
								bool success = true;
								SaveItemMoreTimes(ItemID, count, peer, success, "Purchased from store");
								Player::PlayAudio(peer, "audio/piano_nice.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You've purchased " + to_string(count) + " `o" + getItemDef(ItemID).name + " `wfor `$" + to_string(Price) + " `wGems.\nYou have `$" + to_string(gembux) + " `wGems left.\n\n`5Received: ``" + to_string(count) + " " + getItemDef(ItemID).name + ""));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
							else {
								Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You can't afford `o" + getItemDef(ItemID).name + "``!  You're `$" + to_string(Price - gembux) + "`` Gems short."));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
						}
						if (cch == "action|buy\nitem|5seed\n") {
							/*Fast Item Setup*/
							auto Price = 100;
							auto ItemID = 5706;
							auto count = 1;
							ifstream ifsz("save/gemdb/_" + pData->rawName + ".txt");
							string content((std::istreambuf_iterator<char>(ifsz)), (std::istreambuf_iterator<char>()));
							auto gembux = atoi(content.c_str());
							if (gembux >= Price) {
								if (CheckItemMaxed(peer, ItemID, count) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && CheckItemExists(peer, ItemID) && CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && !CheckItemExists(peer, ItemID)) {
									Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
									GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You don't have enough space in your inventory that. You may be carrying to many of one of the items you are trying to purchase or you don't have enough free spaces to fit them all in your backpack!"));
									ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
									enet_peer_send(peer, 0, packet);
									delete p.data;
									break;
								}
								gembux -= Price;
								ofstream myfile;
								myfile.open("save/gemdb/_" + pData->rawName + ".txt");
								myfile << gembux;
								myfile.close();
								Player::OnSetBux(peer, gembux, 0);
								bool success = true;
								SaveItemMoreTimes(ItemID, count, peer, success, "Purchased from store");
								Player::PlayAudio(peer, "audio/piano_nice.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You've purchased " + to_string(count) + " `o" + getItemDef(ItemID).name + " `wfor `$" + to_string(Price) + " `wGems.\nYou have `$" + to_string(gembux) + " `wGems left.\n\n`5Received: ``" + to_string(count) + " " + getItemDef(ItemID).name + ""));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
							else {
								Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You can't afford `o" + getItemDef(ItemID).name + "``!  You're `$" + to_string(Price - gembux) + "`` Gems short."));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
						}
						if (cch == "action|buy\nitem|farm_pack\n") {
							ifstream ifsz("save/gemdb/_" + pData->rawName + ".txt");
							string content((std::istreambuf_iterator<char>(ifsz)), (std::istreambuf_iterator<char>()));
							auto gembux = atoi(content.c_str());
							if (gembux >= 15000) {
								if (CheckItemMaxed(peer, 872, 1) || CheckItemMaxed(peer, 866, 1) || CheckItemMaxed(peer, 102, 100) || CheckItemMaxed(peer, 340, 40) || CheckItemMaxed(peer, 5666, 40) || CheckItemMaxed(peer, 954, 60) || CheckItemMaxed(peer, 4584, 75) || CheckItemMaxed(peer, 898, 1) || pData->inventory.items.size() + 8 >= pData->currentInventorySize) {
									Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
									GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You don't have enough space in your inventory that. You may be carrying to many of one of the items you are trying to purchase or you don't have enough free spaces to fit them all in your backpack!"));
									ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
									enet_peer_send(peer, 0, packet);
									delete p.data;
									break;
								}
								gembux -= 15000;
								ofstream myfile;
								myfile.open("save/gemdb/_" + pData->rawName + ".txt");
								myfile << gembux;
								myfile.close();
								Player::OnSetBux(peer, gembux, 0);
								bool success = true;
								SaveItemMoreTimes(102, 100, peer, success, "Purchased from store");
								SaveItemMoreTimes(340, 40, peer, success, "Purchased from store");
								SaveItemMoreTimes(5666, 40, peer, success, "Purchased from store");
								SaveItemMoreTimes(954, 60, peer, success, "Purchased from store");
								SaveItemMoreTimes(4584, 75, peer, success, "Purchased from store");
								SaveItemMoreTimes(898, 1, peer, success, "Purchased from store");
								SaveItemMoreTimes(872, 1, peer, success, "Purchased from store");
								SaveItemMoreTimes(866, 1, peer, success, "Purchased from store");
								Player::PlayAudio(peer, "audio/piano_nice.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You've purchased `oFarm Pack `wfor `$" + to_string(15000) + " `wGems.\nYou have `$" + to_string(gembux) + " `wGems left.\n\n`5Received:`` 100 Wooden Platforms`$,`` 40 Chandeliers`$,`` 40 Laser Grids`$,`` 60 Sugar Canes`$,`` 75 Pepper Trees`$,`` Dear John Tractor\n"));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
							else {
								Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You can't afford `oFarm Pack``!  You're `$" + to_string(15000 - gembux) + "`` Gems short."));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
						}
						if (cch == "action|buy\nitem|west_pack\n") {
							ifstream ifsz("save/gemdb/_" + pData->rawName + ".txt");
							string content((std::istreambuf_iterator<char>(ifsz)), (std::istreambuf_iterator<char>()));
							auto gembux = atoi(content.c_str());
							if (gembux >= 8000) {
								if (CheckItemMaxed(peer, 1044, 1) || CheckItemMaxed(peer, 1046, 10) || CheckItemMaxed(peer, 1048, 1) || CheckItemMaxed(peer, 1020, 1) || CheckItemMaxed(peer, 1022, 1) || CheckItemMaxed(peer, 1030, 1) || CheckItemMaxed(peer, 1024, 1) || CheckItemMaxed(peer, 1026, 1) || CheckItemMaxed(peer, 1028, 1) || CheckItemMaxed(peer, 1036, 1) || CheckItemMaxed(peer, 1034, 1) || CheckItemMaxed(peer, 1032, 1) || CheckItemMaxed(peer, 1038, 10) || CheckItemMaxed(peer, 1040, 1) || CheckItemMaxed(peer, 1042, 5) || pData->inventory.items.size() + 15 >= pData->currentInventorySize) {
									Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
									GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You don't have enough space in your inventory that. You may be carrying to many of one of the items you are trying to purchase or you don't have enough free spaces to fit them all in your backpack!"));
									ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
									enet_peer_send(peer, 0, packet);
									delete p.data;
									break;
								}
								gembux -= 8000;
								ofstream myfile;
								myfile.open("save/gemdb/_" + pData->rawName + ".txt");
								myfile << gembux;
								myfile.close();
								Player::OnSetBux(peer, gembux, 0);
								bool success = true;
								//Buffalo`$,`` 10 Rustic Fences`$,`` Campfire`$,`` Parasol
								SaveItemMoreTimes(1022, 1, peer, success, "Purchased from store");
								SaveItemMoreTimes(1030, 1, peer, success, "Purchased from store");
								SaveItemMoreTimes(1024, 1, peer, success, "Purchased from store");
								SaveItemMoreTimes(1026, 1, peer, success, "Purchased from store");
								SaveItemMoreTimes(1028, 1, peer, success, "Purchased from store");
								SaveItemMoreTimes(1036, 1, peer, success, "Purchased form store");
								SaveItemMoreTimes(1034, 1, peer, success, "Purchased from store");
								SaveItemMoreTimes(1032, 1, peer, success, "Purchased from store");
								SaveItemMoreTimes(1038, 10, peer, success, "Purchased from store");
								SaveItemMoreTimes(1040, 1, peer, success, "Purchased from store");
								SaveItemMoreTimes(1042, 5, peer, success, "Purchased form store");
								SaveItemMoreTimes(1044, 1, peer, success, "Purchased from store");
								SaveItemMoreTimes(1046, 10, peer, success, "Purchased from store");
								SaveItemMoreTimes(1048, 1, peer, success, "Purchased from store");
								SaveItemMoreTimes(1020, 1, peer, success, "Purchased from store");
								Player::PlayAudio(peer, "audio/piano_nice.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You've purchased `oWild West Pack `wfor `$" + to_string(8000) + " `wGems.\nYou have `$" + to_string(gembux) + " `wGems left.\n\n`5Received:`` Cowboy Hat`&,`` Cowboy Boots`$,`` War Paint`$,`` Face Bandana`$,`` Sheriff Vest`$,`` Layer Cake Dress`$,`` Corset`$,`` Kansas Curls`$,`` 10 Western Building`$,`` Saloon Doors`$,`` 5 Western Banners`$,`` Buffalo`$,`` 10 Rustic Fences`$,`` Campfire`$,`` Parasol\n"));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
							else {
								Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You can't afford `oWild West Pack``!  You're `$" + to_string(8000 - gembux) + "`` Gems short."));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
						}
						if (cch == "action|buy\nitem|city_pack\n") {
							ifstream ifsz("save/gemdb/_" + pData->rawName + ".txt");
							string content((std::istreambuf_iterator<char>(ifsz)), (std::istreambuf_iterator<char>()));
							auto gembux = atoi(content.c_str());
							if (gembux >= 8000) {
								if (CheckItemMaxed(peer, 1008, 1) || CheckItemMaxed(peer, 986, 3) || CheckItemMaxed(peer, 992, 3) || CheckItemMaxed(peer, 990, 10) || CheckItemMaxed(peer, 996, 10) || CheckItemMaxed(peer, 998, 10) || CheckItemMaxed(peer, 988, 3) || CheckItemMaxed(peer, 1004, 10) || CheckItemMaxed(peer, 1006, 1) || CheckItemMaxed(peer, 1002, 1) || CheckItemMaxed(peer, 994, 10) || pData->inventory.items.size() + 11 >= pData->currentInventorySize) {
									Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
									GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You don't have enough space in your inventory that. You may be carrying to many of one of the items you are trying to purchase or you don't have enough free spaces to fit them all in your backpack!"));
									ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
									enet_peer_send(peer, 0, packet);
									delete p.data;
									break;
								}
								gembux -= 8000;
								ofstream myfile;
								myfile.open("save/gemdb/_" + pData->rawName + ".txt");
								myfile << gembux;
								myfile.close();
								Player::OnSetBux(peer, gembux, 0);
								bool success = true;
								//1 Blue Mailbox`$,`` 1 Fire Hydrant`$,`` 10 Sidewalks
								SaveItemMoreTimes(1008, 1, peer, success, "Purchased from store");
								SaveItemMoreTimes(986, 3, peer, success, "Purchased from store");
								SaveItemMoreTimes(992, 3, peer, success, "Purchased from store");
								SaveItemMoreTimes(990, 10, peer, success, "Purchased from store");
								SaveItemMoreTimes(996, 10, peer, success, "Purchased from store");

								SaveItemMoreTimes(998, 10, peer, success, "Purchased form store");

								SaveItemMoreTimes(988, 3, peer, success, "Purchased from store");

								SaveItemMoreTimes(1004, 10, peer, success, "Purchased from store");

								SaveItemMoreTimes(1006, 1, peer, success, "Purchased from store");

								SaveItemMoreTimes(1002, 1, peer, success, "Purchased from store");
								SaveItemMoreTimes(994, 10, peer, success, "Purchased form store");
								Player::PlayAudio(peer, "audio/piano_nice.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You've purchased `oCity Pack `wfor `$" + to_string(8000) + " `wGems.\nYou have `$" + to_string(gembux) + " `wGems left.\n\n`5Received:`` ATM Machine`&,`` 3 Street Signs`$,`` 3 Streetlamps`$,`` 10 Gothic Building tiles`$,`` 10 Tenement Building tiles`$,`` 10 Fire Escapes`$,`` 3 Gargoyles`$,`` 10 Hedges`$,`` 1 Blue Mailbox`$,`` 1 Fire Hydrant`$,`` 10 Sidewalks\n"));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
							else {
								Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You can't afford `oCity Pack``!  You're `$" + to_string(8000 - gembux) + "`` Gems short."));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
						}
						if (cch == "action|buy\nitem|summer_pack\n") {
							ifstream ifsz("save/gemdb/_" + pData->rawName + ".txt");
							string content((std::istreambuf_iterator<char>(ifsz)), (std::istreambuf_iterator<char>()));
							auto gembux = atoi(content.c_str());
							if (gembux >= 20000) {
								if (CheckItemMaxed(peer, 836, 1) || CheckItemMaxed(peer, 834, 100) || CheckItemMaxed(peer, 830, 1) || pData->inventory.items.size() + 4 >= pData->currentInventorySize) {
									Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
									GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You don't have enough space in your inventory that. You may be carrying to many of one of the items you are trying to purchase or you don't have enough free spaces to fit them all in your backpack!"));
									ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
									enet_peer_send(peer, 0, packet);
									delete p.data;
									break;
								}
								gembux -= 20000;
								ofstream myfile;
								myfile.open("save/gemdb/_" + pData->rawName + ".txt");
								myfile << gembux;
								myfile.close();
								Player::OnSetBux(peer, gembux, 0);
								bool success = true;
								//1 Blue Mailbox`$,`` 1 Fire Hydrant`$,`` 10 Sidewalks
								SaveItemMoreTimes(836, 1, peer, success, "Purchased from store");
								SaveItemMoreTimes(834, 100, peer, success, "Purchased from store");
								SaveItemMoreTimes(830, 1, peer, success, "Purchased from store");
								Player::PlayAudio(peer, "audio/piano_nice.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You've purchased `oSummer Pack `wfor `$" + to_string(20000) + " `wGems.\nYou have `$" + to_string(gembux) + " `wGems left.\n\n`5Received:`` 1 Summer Surprise & 100 Fireworks, 1 Beach Blast\n"));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
							else {
								Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You can't afford `oSummer Pack``!  You're `$" + to_string(20000 - gembux) + "`` Gems short."));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
						}
						if (cch == "action|buy\nitem|science_pack\n") {
							ifstream ifsz("save/gemdb/_" + pData->rawName + ".txt");
							string content((std::istreambuf_iterator<char>(ifsz)), (std::istreambuf_iterator<char>()));
							auto gembux = atoi(content.c_str());
							if (gembux >= 5000) {
								if (CheckItemMaxed(peer, 916, 5) || CheckItemMaxed(peer, 914, 10) || CheckItemMaxed(peer, 924, 5) || CheckItemMaxed(peer, 920, 5) || CheckItemMaxed(peer, 918, 5) || CheckItemMaxed(peer, 930, 1) || CheckItemMaxed(peer, 928, 1) || CheckItemMaxed(peer, 912, 1) || CheckItemMaxed(peer, 772, 1) || CheckItemMaxed(peer, 770, 1) || CheckItemMaxed(peer, 904, 1) || pData->inventory.items.size() + 11 >= pData->currentInventorySize) {
									Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
									GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You don't have enough space in your inventory that. You may be carrying to many of one of the items you are trying to purchase or you don't have enough free spaces to fit them all in your backpack!"));
									ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
									enet_peer_send(peer, 0, packet);
									delete p.data;
									break;
								}
								gembux -= 5000;
								ofstream myfile;
								myfile.open("save/gemdb/_" + pData->rawName + ".txt");
								myfile << gembux;
								myfile.close();
								Player::OnSetBux(peer, gembux, 0);
								bool success = true;
								SaveItemMoreTimes(930, 1, peer, success, "Purchased from store");
								SaveItemMoreTimes(928, 1, peer, success, "Purchased from store");
								SaveItemMoreTimes(912, 1, peer, success, "Purchased from store");
								SaveItemMoreTimes(772, 1, peer, success, "Purchased from store");
								SaveItemMoreTimes(770, 1, peer, success, "Purchased from store");
								SaveItemMoreTimes(904, 1, peer, success, "Purchased form store");
								SaveItemMoreTimes(916, 5, peer, success, "Purchased from store");
								SaveItemMoreTimes(914, 10, peer, success, "Purchased from store");
								SaveItemMoreTimes(924, 5, peer, success, "Purchased from store");
								SaveItemMoreTimes(920, 5, peer, success, "Purchased from store");
								SaveItemMoreTimes(918, 5, peer, success, "Purchased form store");
								Player::PlayAudio(peer, "audio/piano_nice.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You've purchased `oMad Science Kit `wfor `$" + to_string(5000) + " `wGems.\nYou have `$" + to_string(gembux) + " `wGems left.\n\n`5Received:`` Death Ray`&,`` Science Station`$,`` Laboratory`$,`` LabCoat`$,`` Combover Hair`$,`` Goggles`$,`` 5 Chemical R`$,`` 10 Chemical G`$,`` 5 Chemical Y`$,`` 5 Chemical B`$,`` 5 Chemical P\n"));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
							else {
								Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You can't afford `oMad Science Kit``!  You're `$" + to_string(5000 - gembux) + "`` Gems short."));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
						}
						if (cch == "action|buy\nitem|fishin_pack\n") {
							ifstream ifsz("save/gemdb/_" + pData->rawName + ".txt");
							string content((std::istreambuf_iterator<char>(ifsz)), (std::istreambuf_iterator<char>()));
							auto gembux = atoi(content.c_str());
							if (gembux >= 10000) {
								if (CheckItemMaxed(peer, 3044, 1) || CheckItemMaxed(peer, 2914, 5) || CheckItemMaxed(peer, 5522, 1) || CheckItemMaxed(peer, 5524, 1) || CheckItemMaxed(peer, 2912, 1) || CheckItemMaxed(peer, 3004, 10) || CheckItemMaxed(peer, 3002, 1) || CheckItemMaxed(peer, 822, 5) || pData->inventory.items.size() + 8 >= pData->currentInventorySize) {
									Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
									GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You don't have enough space in your inventory that. You may be carrying to many of one of the items you are trying to purchase or you don't have enough free spaces to fit them all in your backpack!"));
									ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
									enet_peer_send(peer, 0, packet);
									delete p.data;
									break;
								}
								gembux -= 10000;
								ofstream myfile;
								myfile.open("save/gemdb/_" + pData->rawName + ".txt");
								myfile << gembux;
								myfile.close();
								Player::OnSetBux(peer, gembux, 0);
								bool success = true;
								SaveItemMoreTimes(3044, 1, peer, success, "Purchased from store");
								SaveItemMoreTimes(2914, 5, peer, success, "Purchased from store");
								SaveItemMoreTimes(5522, 1, peer, success, "Purchased from store");
								SaveItemMoreTimes(5524, 1, peer, success, "Purchased from store");
								SaveItemMoreTimes(2912, 1, peer, success, "Purchased from store");
								SaveItemMoreTimes(3004, 10, peer, success, "Purchased from store");
								SaveItemMoreTimes(3002, 1, peer, success, "Purchased from store");
								SaveItemMoreTimes(822, 5, peer, success);
								Player::PlayAudio(peer, "audio/piano_nice.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You've purchased `oFishin' Pack `wfor `$" + to_string(10000) + " `wGems.\nYou have `$" + to_string(gembux) + " `wGems left.\n\n`5Received:`` Tackle Box`$,`` 5 Wiggly Worms`$,`` Hand Drill`$,`` Nuclear Detonator`$,`` Fishing Rod`$,`` 10 Fish Tanks`$,`` Fish Tank Port`$,`` 5 Water Buckets\n"));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
							else {
								Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You can't afford `oFishin' Pack``!  You're `$" + to_string(10000 - gembux) + "`` Gems short."));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
						}
						if (cch == "action|buy\nitem|surgical_kit\n") {
							ifstream ifsz("save/gemdb/_" + pData->rawName + ".txt");
							string content((std::istreambuf_iterator<char>(ifsz)), (std::istreambuf_iterator<char>()));
							auto gembux = atoi(content.c_str());
							if (gembux >= 12000) {
								if (CheckItemMaxed(peer, 1258, 5) || CheckItemMaxed(peer, 1260, 5) || CheckItemMaxed(peer, 1262, 5) || CheckItemMaxed(peer, 1264, 5) || CheckItemMaxed(peer, 1266, 5) || CheckItemMaxed(peer, 1268, 5) || CheckItemMaxed(peer, 1270, 5) || CheckItemMaxed(peer, 1256, 1) || CheckItemMaxed(peer, 1240, 1) || CheckItemMaxed(peer, 4308, 5) || CheckItemMaxed(peer, 4310, 5) || CheckItemMaxed(peer, 4312, 5) || CheckItemMaxed(peer, 4314, 5) || CheckItemMaxed(peer, 4316, 5) || CheckItemMaxed(peer, 4318, 5) || CheckItemMaxed(peer, 8558, 1) || CheckItemMaxed(peer, 8500, 10) || pData->inventory.items.size() + 18 >= pData->currentInventorySize) {
									Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
									GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You don't have enough space in your inventory that. You may be carrying to many of one of the items you are trying to purchase or you don't have enough free spaces to fit them all in your backpack!"));
									ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
									enet_peer_send(peer, 0, packet);
									delete p.data;
									break;
								}
								gembux -= 12000;
								ofstream myfile;
								myfile.open("save/gemdb/_" + pData->rawName + ".txt");
								myfile << gembux;
								myfile.close();
								Player::OnSetBux(peer, gembux, 0);
								bool success = true;
								SaveItemMoreTimes(1258, 5, peer, success, pData->rawName + " from surgery tool pack");
								SaveItemMoreTimes(1260, 5, peer, success, pData->rawName + " from surgery tool pack");
								SaveItemMoreTimes(1262, 5, peer, success, pData->rawName + " from surgery tool pack");
								SaveItemMoreTimes(1264, 5, peer, success, pData->rawName + " from surgery tool pack");
								SaveItemMoreTimes(1266, 5, peer, success, pData->rawName + " from surgery tool pack");
								SaveItemMoreTimes(1268, 5, peer, success, pData->rawName + " from surgery tool pack");
								SaveItemMoreTimes(1270, 5, peer, success, pData->rawName + " from surgery tool pack");
								SaveItemMoreTimes(4308, 5, peer, success, pData->rawName + " from surgery tool pack");
								SaveItemMoreTimes(4310, 5, peer, success, pData->rawName + " from surgery tool pack");
								SaveItemMoreTimes(4312, 5, peer, success, pData->rawName + " from surgery tool pack");
								SaveItemMoreTimes(4314, 5, peer, success, pData->rawName + " from surgery tool pack");
								SaveItemMoreTimes(4316, 5, peer, success, pData->rawName + " from surgery tool pack");
								SaveItemMoreTimes(4318, 5, peer, success, pData->rawName + " from surgery tool pack");
								SaveItemMoreTimes(8558, 1, peer, success, pData->rawName + " from surgery tool pack");
								SaveItemMoreTimes(8500, 10, peer, success, pData->rawName + " from surgery tool pack");
								SaveItemMoreTimes(1240, 1, peer, success, pData->rawName + " from surgery tool pack");
								SaveItemMoreTimes(1256, 1, peer, success, pData->rawName + " from surgery tool pack");
								Player::PlayAudio(peer, "audio/piano_nice.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You've purchased `oFishin' Pack `wfor `$" + to_string(12000) + " `wGems.\nYou have `$" + to_string(gembux) + " `wGems left.\n\n`5Received:`` Heart Monitor`$,`` Hospital Bed`$,`` 10 Med-A-Check`$,`` Train-E`$,`` 65 Surgical Tools\n"));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
							else {
								Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You can't afford `oFishin' Pack``!  You're `$" + to_string(12000 - gembux) + "`` Gems short."));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
						}
						if (cch == "action|buy\nitem|firefighter\n") {
							ifstream ifsz("save/gemdb/_" + pData->rawName + ".txt");
							string content((std::istreambuf_iterator<char>(ifsz)), (std::istreambuf_iterator<char>()));
							auto gembux = atoi(content.c_str());
							if (gembux >= 10000) {
								if (CheckItemMaxed(peer, 3048, 1) || CheckItemMaxed(peer, 3056, 1) || CheckItemMaxed(peer, 3060, 1) || CheckItemMaxed(peer, 3052, 1) || CheckItemMaxed(peer, 3066, 1) || CheckItemMaxed(peer, 3072, 1) || pData->inventory.items.size() + 6 >= pData->currentInventorySize) {
									Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
									GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You don't have enough space in your inventory that. You may be carrying to many of one of the items you are trying to purchase or you don't have enough free spaces to fit them all in your backpack!"));
									ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
									enet_peer_send(peer, 0, packet);
									delete p.data;
									break;
								}
								gembux -= 10000;
								ofstream myfile;
								myfile.open("save/gemdb/_" + pData->rawName + ".txt");
								myfile << gembux;
								myfile.close();
								Player::OnSetBux(peer, gembux, 0);
								bool success = true;
								SaveItemMoreTimes(3048, 1, peer, success);
								SaveItemMoreTimes(3056, 1, peer, success);
								SaveItemMoreTimes(3060, 1, peer, success);
								SaveItemMoreTimes(3052, 1, peer, success);
								SaveItemMoreTimes(3066, 1, peer, success);
								SaveItemMoreTimes(3072, 1, peer, success);
								Player::PlayAudio(peer, "audio/piano_nice.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You've purchased `oFirefighter Pack `wfor `$" + to_string(10000) + " `wGems.\nYou have `$" + to_string(gembux) + " `wGems left.\n\n`5Received:`` Firefighter Helmet - Yellow`$,`` Firefighter Pants - Yellow`$,`` Firefighter Jacket - Yellow`$,`` Firefighter Boots`$,`` Fire Hose`$,`` Firehouse\n"));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
							else {
								Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You can't afford `oFirefighter Pack``!  You're `$" + to_string(10000 - gembux) + "`` Gems short."));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
						}
						if (cch == "action|buy\nitem|paleo_kit\n") {
							ifstream ifsz("save/gemdb/_" + pData->rawName + ".txt");
							string content((std::istreambuf_iterator<char>(ifsz)), (std::istreambuf_iterator<char>()));
							auto gembux = atoi(content.c_str());
							if (gembux >= 20000) {
								if (CheckItemMaxed(peer, 4132, 5) || CheckItemMaxed(peer, 3932, 1) || CheckItemMaxed(peer, 3934, 1) || CheckItemMaxed(peer, 3938, 1) || CheckItemMaxed(peer, 4128, 1) || pData->inventory.items.size() + 5 >= pData->currentInventorySize) {
									Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
									GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You don't have enough space in your inventory that. You may be carrying to many of one of the items you are trying to purchase or you don't have enough free spaces to fit them all in your backpack!"));
									ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
									enet_peer_send(peer, 0, packet);
									delete p.data;
									break;
								}
								gembux -= 20000;
								ofstream myfile;
								myfile.open("save/gemdb/_" + pData->rawName + ".txt");
								myfile << gembux;
								myfile.close();
								Player::OnSetBux(peer, gembux, 0);
								bool success = true;
								SaveItemMoreTimes(4132, 5, peer, success, "Purchased from store");
								SaveItemMoreTimes(3932, 1, peer, success, "Purchased from store");
								SaveItemMoreTimes(3934, 1, peer, success, "Purchased from store");
								SaveItemMoreTimes(3938, 1, peer, success, "Purchased from store");
								SaveItemMoreTimes(4128, 1, peer, success, "Purchased from store");
								Player::PlayAudio(peer, "audio/piano_nice.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You've purchased `oPaleontologist's Kit `wfor `$" + to_string(20000) + " `wGems.\nYou have `$" + to_string(gembux) + " `wGems left.\n\n`5Received:`` Fossil Prep Station`$,`` 5 Fossil Brushes`$,`` Rock Hammer`$,`` Rock Chisel`$,`` Blue Hardhat\n"));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
							else {
								Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You can't afford `oPaleontologist's Kit``!  You're `$" + to_string(20000 - gembux) + "`` Gems short."));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
						}
						if (cch == "action|buy\nitem|token\n") {
							auto KiekTuri = 0;
							try {
								for (auto i = 0; i < pData->inventory.items.size(); i++) {
									if (pData->inventory.items.at(i).itemID == 1486 && pData->inventory.items.at(i).itemCount >= 1) {
										KiekTuri = pData->inventory.items.at(i).itemCount;
										break;
									}
								}
							}
							catch (const std::out_of_range& e) {
								std::cout << e.what() << std::endl;
							}
							GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStoreRequest"), "set_description_text|`2Spend your Growtokens!`` (You have `5" + to_string(KiekTuri) + "``) You earn Growtokens from Crazy Jim and Sales-Man. Select the item you'd like more info on, or BACK to go back.\nenable_tabs|1\nadd_tab_button|main_menu|Home|interface/large/btn_shop2.rttex||0|0|0|0||||-1|-1|||0|\nadd_tab_button|locks_menu|Locks And Stuff|interface/large/btn_shop2.rttex||0|1|0|0||||-1|-1|||0|\nadd_tab_button|itempack_menu|Item Packs|interface/large/btn_shop2.rttex||0|3|0|0||||-1|-1|||0|\nadd_tab_button|bigitems_menu|Awesome Items|interface/large/btn_shop2.rttex||0|4|0|0||||-1|-1|||0|\nadd_tab_button|weather_menu|Weather Machines|interface/large/btn_shop2.rttex|Tired of the same sunny sky?  We offer alternatives within...|0|5|0|0||||-1|-1|||0|\nadd_tab_button|token_menu|Growtoken Items|interface/large/btn_shop2.rttex||1|2|0|0||||-1|-1|||0|\nadd_button|challenge_timer|`oChallenge Timer``|interface/large/store_buttons/store_buttons15.rttex|`2You Get:`` 1 Challenge Timer.<CR><CR>`5Description:`` Get more people playing your parkours with this secure prize system. You'll need a `#Challenge Start Flag`` and `#Challenge End Flag`` as well (not included). Stock prizes into the Challenge Timer, set a time limit, and watch as players race from start to end. If they make it in time, they win a prize!|0|5|-5|0|||-1|-1||-1|-1||1||||||0|\nadd_button|xp_potion|`oExperience Potion``|interface/large/store_buttons/store_buttons9.rttex|`2You Get:`` 1 Experience Potion.<CR><CR>`5Description:`` This `#Untradeable`` delicious fizzy drink will make you smarter! 10,000 XP smarter instantly, to be exact.|0|2|-10|0|||-1|-1||-1|-1||1||||||0|\nadd_button|megaphone|`oMegaphone``|interface/large/store_buttons/store_buttons15.rttex|`2You Get:`` 1 Megaphone.<CR><CR>`5Description:`` You like broadcasting messages, but you're not so big on spending gems? Buy a Megaphone with Growtokens! Each Megaphone can be used once to send a super broadcast to all players in the game.|0|7|-10|0|||-1|-1||-1|-1||1||||||0|\nadd_button|growmoji_pack|`oGrowmoji Mystery Box``|interface/large/store_buttons/store_buttons19.rttex|`2You Get:`` 1 Growmoji.<CR><CR>`5Description:`` Express yourself! This mysterious box contains one of five fun growmojis you can use to spice up your chat! Which will you get?|0|1|-15|0|||-1|-1||-1|-1||1||||||0|\nadd_button|mini_mod|`oMini-Mod``|interface/large/store_buttons/store_buttons17.rttex|`2You Get:`` 1 Mini-Mod.<CR><CR>`5Description:`` Oh no, it's a Mini-Mod! Punch him to activate (you'll want to punch him!). When activated, he won't allow anyone to drop items in your world.|0|0|-20|0|||-1|-1||-1|-1||1||||||0|\nadd_button|derpy_star|`oDerpy Star Block``|interface/large/store_buttons/store_buttons10.rttex|`2You Get:`` 1 Derpy Star Block.<CR><CR>`5Description:`` DER IM A SUPERSTAR. This is a fairly ordinary block, except for the derpy star on it. Note: it is not permanent, and it doesn't drop seeds. So use it wisely!|0|3|-30|0|||-1|-1||-1|-1||1||||||0|\nadd_button|dirt_gun|`oBLYoshi's Free Dirt``|interface/large/store_buttons/store_buttons13.rttex|`2You Get:`` 1 BLYoshi's Free Dirt.<CR><CR>`5Description:`` 'Free' might be stretching it, but hey, once you buy this deadly rifle, you can spew out all the dirt you want for free! Note: the dirt is launched at high velocity and explodes on impact. Sponsored by BLYoshi.|0|4|-40|0|||-1|-1||-1|-1||1||||||0|\nadd_button|nothingness|`oWeather Machine - Nothingness``|interface/large/store_buttons/store_buttons9.rttex|`2You Get:`` 1 Weather Machine - Nothingness.<CR><CR>`5Description:`` Tired of all that fancy weather?  This machine will turn your world completely black. Yup, that's it. Not a single pixel in the background except pure blackness.|0|3|-50|0|||-1|-1||-1|-1||1||||||0|\nadd_button|spike_juice|`oSpike Juice``|interface/large/store_buttons/store_buttons10.rttex|`2You Get:`` 1 Spike Juice.<CR><CR>`5Description:`` It's fresh squeezed, with little bits of spikes still in it! Drinking this `#Untradeable`` one-use potion will make you immune to Death Spikes and Lava for 5 seconds.|0|5|-60|0|||-1|-1||-1|-1||1||||||0|\nadd_button|doodad|`oDoodad``|interface/large/store_buttons/store_buttons9.rttex|`2You Get:`` 1 Doodad.<CR><CR>`5Description:`` I have no idea what this thing does. It's something electronic? Maybe?|0|5|-75|0|||-1|-1||-1|-1||1||||||0|\nadd_button|crystal_cape|`oCrystal Cape``|interface/large/store_buttons/store_buttons11.rttex|`2You Get:`` 1 Crystal Cape.<CR><CR>`5Description:`` This cape is woven of pure crystal, which makes it pretty uncomfortable. But it also makes it magical! It lets you double-jump off of an imaginary Crystal Block in mid-air. Sponsored by Edvoid20, HemeTems, and Aboge.|0|5|-90|0|||-1|-1||-1|-1||1||||||0|\nadd_button|focused_eyes|`oFocused Eyes``|interface/large/store_buttons/store_buttons9.rttex|`2You Get:`` 1 Focused Eyes.<CR><CR>`5Description:`` This `#Untradeable`` item lets you shoot electricity from your eyes! Wear them with pride, and creepiness.|0|4|-100|0|||-1|-1||-1|-1||1||||||0|\nadd_button|grip_tape|`oGrip Tape``|interface/large/store_buttons/store_buttons14.rttex|`2You Get:`` 1 Grip Tape.<CR><CR>`5Description:`` This is handy for wrapping around the handle of a weapon or tool. It can improve your grip, as well as protect you from cold metal handles. If you aren't planning to craft a weapon that requires Grip Tape, this does you no good at all!|0|5|-100|0|||-1|-1||-1|-1||1||||||0|\nadd_button|cat_eyes|`oCat Eyes``|interface/large/store_buttons/store_buttons23.rttex|`2You Get:`` 1 Cat Eyes.<CR><CR>`5Description:`` Wow, pawesome! These new eyes are the cat's meow, and the purrfect addition to any style.|0|5|-100|0|||-1|-1||-1|-1||1||||||0|\nadd_button|night_vision|`oNight Vision Goggles``|interface/large/store_buttons/store_buttons15.rttex|`2You Get:`` 1 Night Vision Goggles.<CR><CR>`5Description:`` Scared of the dark? We have a solution. You can wear these goggles just to look cool, but if you also happen to have a D Battery (`4batteries not included``) on you, you will be able to see through darkness like it's not even there! Each D Battery can power your goggles for 1 minute. `2If you are in a world you own, the goggles will not require batteries!`` Note: you can't turn the goggles off without removing them, so you'll be wasting your battery if you wear them in daylight while carrying D Batteries.|0|3|-110|0|||-1|-1||-1|-1||1||||||0|\nadd_button|muddy_pants|`oMuddy Pants``|interface/large/store_buttons/store_buttons12.rttex|`2You Get:`` 1 Muddy Pants.<CR><CR>`5Description:`` Well, this is just a pair of muddy pants. But it does come with a super secret bonus surprise that is sure to blow your mind!|0|7|-125|0|||-1|-1||-1|-1||1||||||0|\nadd_button|piranha|`oCuddly Piranha``|interface/large/store_buttons/store_buttons10.rttex|`2You Get:`` 1 Cuddly Piranha.<CR><CR>`5Description:`` This friendly pet piranha won't stay in its bowl!  It just wants to snuggle with your face!|0|0|-150|0|||-1|-1||-1|-1||1||||||0|\nadd_button|puddy_leash|`oPuddy Leash``|interface/large/store_buttons/store_buttons11.rttex|`2You Get:`` 1 Puddy Leash.<CR><CR>`5Description:`` Puddy is a friendly little kitten who will follow you around forever.|0|7|-180|0|||-1|-1||-1|-1||1||||||0|\nadd_button|golden_axe|`oGolden Pickaxe``|interface/large/store_buttons/store_buttons9.rttex|`2You Get:`` 1 Golden Pickaxe.<CR><CR>`5Description:`` Get your own sparkly pickaxe! This `#Untradeable`` item is a status symbol! Oh sure, it isn't any more effective than a normal pickaxe, but it sparkles!|0|1|-200|0|||-1|-1||-1|-1||1||||||0|\nadd_button|puppy_leash|`oPuppy Leash``|interface/large/store_buttons/store_buttons11.rttex|`2You Get:`` 1 Puppy Leash.<CR><CR>`5Description:`` Get your own pet puppy! This little dog will follow you around forever, never wavering in her loyalty, thus making her `#Untradeable``.|0|4|-200|0|||-1|-1||-1|-1||1||||||0|\nadd_button|diggers_spade|`oDigger's Spade``|interface/large/store_buttons/store_buttons13.rttex|`2You Get:`` 1 Digger's Spade.<CR><CR>`5Description:`` This may appear to be a humble shovel, but in fact it is enchanted with the greatest magic in Growtopia. It can smash Dirt or Cave Background in a single hit! Unfortunately, it's worthless at digging through anything else. Note: The spade is `#UNTRADEABLE``.|0|7|-200|0|||-1|-1||-1|-1||1||||||0|\nadd_button|meow_ears|`oMeow Ears``|interface/large/store_buttons/store_buttons22.rttex|`2You Get:`` 1 Meow Ears.<CR><CR>`5Description:`` Meow's super special ears that everyone can now get! Note: These ears are `#UNTRADEABLE``.|0|0|-200|0|||-1|-1||-1|-1||1||||||0|\nadd_button|frosty_hair|`oFrosty Hair``|interface/large/store_buttons/store_buttons23.rttex|`2You Get:`` 1 Frosty Hair.<CR><CR>`5Description:`` Coldplay is cold, but you can be freezing! Note: The frosty hair is `#UNTRADEABLE``.|0|0|-200|0|||-1|-1||-1|-1||1||||||0|\nadd_button|zerkon_helmet|`oEvil Space Helmet``|interface/large/store_buttons/store_buttons21.rttex|`2You Get:`` 1 Evil Space Helmet.<CR><CR>`5Description:`` Zerkon commands a starship too small to actually board - pah, time to rule the galaxy properly! Note: The evil space helmet is `#UNTRADEABLE``.|0|6|-200|0|||-1|-1||-1|-1||1||||||0|\nadd_button|seils_magic_orb|`oSeil's Magic Orbs``|interface/large/store_buttons/store_buttons21.rttex|`2You Get:`` 1 Seil's Magic Orbs.<CR><CR>`5Description:`` Seil is some kind of evil wizard, now you can be too! Note: These magic orbs are `#UNTRADEABLE``.|0|7|-200|0|||-1|-1||-1|-1||1||||||0|\nadd_button|atomic_shadow_scythe|`oAtomic Shadow Scythe``|interface/large/store_buttons/store_buttons21.rttex|`2You Get:`` 1 Atomic Shadow Scythe.<CR><CR>`5Description:`` AtomicShadow might actually be evil, now you can try it out! Note: The shadow scythe is `#UNTRADEABLE``.|0|5|-200|0|||-1|-1||-1|-1||1||||||0|\nadd_button|poseidon_diggers_trident|`oPoseidon's Digger's Trident``|interface/large/store_buttons/store_buttons25.rttex|`2You Get:`` 1 Poseidon's Digger's Trident.<CR><CR>`5Description:`` A gift from the gods. This may appear to be a humble trident, but in fact it has the power of Poseidon himself. It can smash `8Deep Sand`` or `8Ocean Rock`` in a single hit. Unfortunately, you don't get to wield the full might of Poseidon... the trident is worthless at smashing anything else. Note: The trident is `#UNTRADEABLE``.|0|6|-200|0|||-1|-1||-1|-1||1||||||0|\nadd_button|grow_boy|`oGrowBoy``|interface/large/store_buttons/store_buttons32.rttex|`2You Get:`` 1 Growboy.<CR><CR>`5Description:`` Bask in the nostalgic green screened goodness of the Growboy! A portable gaming device that packs a punch. Now you're playing with GrowPower! Note: The Growboy is `#UNTRADEABLE``.|0|2|-100|0|||-1|-1||-1|-1||1||||||0|\nadd_button|tsed|`oTactical Stealth Espionage Device``|interface/large/store_buttons/store_buttons32.rttex|`2You Get:`` 1 Tactical Stealth Espionage Device.<CR><CR>`5Description:`` This is Growtech's latest innovation on tactical espionage! Using the latest in scientific breakthroughs this device allows you to seamlessly disguise yourself as... a cardboard box! Note: The Tactical Stealth Espionage Device is `#UNTRADEABLE``.|0|1|-150|0|||-1|-1||-1|-1||1||||||0|\nadd_button|really_dangerous_pet_llama|`oReally Dangerous Pet Llama``|interface/large/store_buttons/store_buttons32.rttex|`2You Get:`` 1 Really Dangerous Pet Llama.<CR><CR>`5Description:`` This Llama is ready for anything Growtopia throws at it! Armed with a silo of Growtech Missiles, experimental Growtech hardened steel armor and a rather snazzy helmet (Llama's own) this Llama is Dangerous with a capital D! Note: This Really Dangerous Pet Llama is `#UNTRADEABLE``.|0|0|-200|0|||-1|-1||-1|-1||1||||||0|"));
							ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
							enet_peer_send(peer, 0, packet);
							delete p.data;
						}
						if (cch == "action|buy\nitem|itempack\n") {
							string dialog = "set_description_text|`2Item Packs!``  Select the item you'd like more info on, or BACK to go back.\nenable_tabs|1\nadd_tab_button|main_menu|Home|interface/large/btn_shop2.rttex||0|0|0|0||||-1|-1|||0|\nadd_tab_button|locks_menu|Locks And Stuff|interface/large/btn_shop2.rttex||0|1|0|0||||-1|-1|||0|\nadd_tab_button|itempack_menu|Item Packs|interface/large/btn_shop2.rttex||1|3|0|0||||-1|-1|||0|\nadd_tab_button|bigitems_menu|Awesome Items|interface/large/btn_shop2.rttex||0|4|0|0||||-1|-1|||0|\nadd_tab_button|weather_menu|Weather Machines|interface/large/btn_shop2.rttex|Tired of the same sunny sky?  We offer alternatives within...|0|5|0|0||||-1|-1|||0|\nadd_tab_button|token_menu|Growtoken Items|interface/large/btn_shop2.rttex||0|2|0|0||||-1|-1|||0|\nadd_button|world_lock|`oWorld Lock``|interface/large/store_buttons/store_buttons.rttex|`2You Get:`` 1 World Lock.<CR><CR>`5Description:`` Become the undisputed ruler of your domain with one of these babies.  It works like a normal lock except it locks the `$entire world``!  Won't work on worlds that other people already have locks on. You can even add additional normal locks to give access to certain areas to friends. `5It's a perma-item, is never lost when destroyed.``  `wRecycles for 200 Gems.``|0|7|2000|0|||-1|-1||-1|-1||1||||||0|\nadd_button|world_lock_10_pack|`oWorld Lock Pack``|interface/large/store_buttons/store_buttons18.rttex|`2You Get:`` 10 World Locks.<CR><CR>`5Description:`` 10-pack of World Locks. Become the undisputed ruler of up to TEN kingdoms with these babies. Each works like a normal lock except it locks the `$entire world``!  Won't work on worlds that other people already have locks on. You can even add additional normal locks to give access to certain areas to friends. `5It's a perma-item, is never lost when destroyed.`` `wEach recycles for 200 Gems.``|0|3|20000|0|||-1|-1||-1|-1||1||||||0|\nadd_button|door_pack|`oDoor And Sign Hello Pack``|interface/large/store_buttons/store_buttons.rttex|`2You Get:`` 1 Door and 1 Sign.<CR><CR>`5Description:`` Own your very own door and sign! This pack comes with one of each. Leave cryptic messages and create a door that can open to, well, anywhere.|0|3|15|0|||-1|-1||-1|-1||1||||||0|\nadd_button|door_mover|`oDoor Mover``|interface/large/store_buttons/store_buttons8.rttex|`2You Get:`` 1 Door Mover.<CR><CR>`5Description:`` Unsatisfied with your world's layout?  This one-use device can be used to move the White Door to any new location in your world, provided there are 2 empty spaces for it to fit in. Disappears when used. `2Only usable on a world you have World Locked.``|0|6|5000|0|||-1|-1||-1|-1||1||||||0|\nadd_button|vending_machine|`oVending Machine``|interface/large/store_buttons/store_buttons13.rttex|`2You Get:`` 1 Vending Machine.<CR><CR>`5Description:`` Tired of interacting with human beings? Try a Vending Machine! You can put a stack of items inside it, set a price in World Locks, and people can buy from the machine while you sit back and rake in the profits! `5It's a perma-item, is never lost when destroyed, and it is not available any other way.``|0|6|8000|0|||-1|-1||-1|-1||1||||||0|\nadd_button|digi_vend|`oDigiVend Machine``|interface/large/store_buttons/store_buttons29.rttex|`2You Get:`` 1 DigiVend Machine.<CR><CR>`5Description:`` Get with the times and go digital! This wired vending machine can connect its contents to Vending Hubs AND the multiversal economy, providing a unified shopping experience along with price checks to help you sell your goods! All that, and still no human-related hassle! Use your wrench on this to stock it with an item and set a price in World Locks. Other players will be able to buy from it! Only works in World-Locked worlds.|0|2|12000|0|||-1|-1||-1|-1||1||||||0|\nadd_button|checkout_counter|`oVending Hub - Checkout Counter``|interface/large/store_buttons/store_buttons29.rttex|`2You Get:`` 1 Vending Hub.<CR><CR>`5Description:`` Your one-stop shop! This vending hub will collect and display (and let shoppers buy) the contents of ALL DigiVends in its row or column (wrench it to set which the direction)! Wow! Now that's a shopping experience we can all enjoy! Note: Only works in World-Locked worlds.|0|3|50000|0|||-1|-1||-1|-1||1||||||0|\nadd_button|change_addr|`oChange of Address``|interface/large/store_buttons/store_buttons12.rttex|`2You Get:`` 1 Change of Address.<CR><CR>`5Description:`` Don't like the name of your world? You can use up one of these to trade your world's name with the name of any other world that you own. You must have a `2World Lock`` in both worlds. Go lock up that empty world with the new name you want and swap away!|0|6|20000|0|||-1|-1||-1|-1||1||||||0|\nadd_button|signal_jammer|`oSignal Jammer``|interface/large/store_buttons/store_buttons.rttex|`2You Get:`` 1 Signal Jammer.<CR><CR>`5Description:`` Get off the grid! Install a `$Signal Jammer``! A single punch will cause it to whir to life, tireless hiding your world and its population from pesky snoopers - only those who know the world name will be able to enter. `5It's a perma-item, is never lost when destroyed.``|1|6|2000|0|||-1|-1||-1|-1||1||||||0|\nadd_button|punch_jammer|`oPunch Jammer``|interface/large/store_buttons/store_buttons7.rttex|`2You Get:`` 1 Punch Jammer.<CR><CR>`5Description:`` Tired of getting bashed around? Set up a Punch Jammer in your world, and people won't be able to punch each other! Can be turned on and off as needed. `5It's a perma-item, is never lost when destroyed.``|0|4|15000|0|||-1|-1||-1|-1||1||||||0|\nadd_button|zombie_jammer|`oZombie Jammer``|interface/large/store_buttons/store_buttons7.rttex|`2You Get:`` 1 Zombie Jammer.<CR><CR>`5Description:`` Got a parkour or race that you don't want slowed down? Turn this on and nobody can be infected by zombie bites in your world. It does not prevent direct infection by the g-Virus itself though. `5It's a perma-item, is never lost when destroyed.``|0|5|15000|0|||-1|-1||-1|-1||1||||||0|\nadd_button|starship_blast|`oImperial Starship Blast``|interface/large/store_buttons/store_buttons21.rttex|`2You Get:`` 1 Imperial Starship Blast.<CR><CR>`5Description:`` Command your very own Starship and explore the cosmos! This blast contains one of 3 possible Imperial ship types - which will you get? Note: Each Starship comes with a full tank of gas, an Imperial Helm - Mk. I, Imperial Reactor - Mk. I and an Imperial Viewscreen - Mk. I, so you'll be all set for your adventure among the stars! Note: A Starship also comes with an assortment of space-age blocks!|0|1|10000|0|||-1|-1||-1|-1||1||||||0|\nadd_button|surg_blast|`oSurgWorld Blast``|interface/large/store_buttons/store_buttons27.rttex|`2You Get:`` 1 SurgWorld Blast and 1 Caduceaxe.<CR><CR>`5Description:`` Your gateway to a world of medical wonders! SurgWorld is a place of care and healing, with all kinds of interesting blocks, top tips on how to treat people with surgery, and an increased chance of getting maladies while you work! Also comes with 1 Caduceaxe to extract Vaccine Drops from blocks. `6Warning:`` May break when extracting vaccine.|0|2|10000|0|||-1|-1||-1|-1||1||||||0|\nadd_button|bountiful_blast|`oBountiful Blast``|interface/large/store_buttons/store_buttons27.rttex|`2You Get:`` 1 Bountiful Blast.<CR><CR>`5Description:`` Enter a world of fertile soil, cheerful sunshine and lush green hills, and bountiful new trees! This blast is your ticket to a different kind of farming!|0|3|5000|0|||-1|-1||-1|-1||1||||||0|\nadd_button|thermo_blast|`oThermonuclear Blast``|interface/large/store_buttons/store_buttons8.rttex|`2You Get:`` 1 Thermonuclear Blast.<CR><CR>`5Description:`` This supervillainous device will blast you to a new world that has been scoured completely empty - it contains nothing but Bedrock and a White Door. Remember: When using this, you are creating a NEW world by typing in a new name. It would be irresponsible to let you blow up an entire existing world.|0|5|15000|0|||-1|-1||-1|-1||1||||||0|\nadd_button|antigravity_generator|`oAntigravity Generator``|interface/large/store_buttons/store_buttons17.rttex|`2You Get:`` 1 Antigravity Generator.<CR><CR>`5Description:`` Disables gravity in your world when activated! Well, it reduces gravity, and lets everybody jump as much as they want! `5It's a perma-item - never lost when destroyed! `4Not available any other way!````|0|3|450000|0|||-1|-1||-1|-1||1||||||0|";
							GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStoreRequest"), dialog + "\nadd_button|safe_vault|`oSafe Vault``|interface/large/store_buttons/store_buttons27.rttex|`2You Get:`` 1 Safe Vault.<CR><CR>`5Description:`` Protect your items with a password with GrowTech's new Safe Vault!|0|4|10000|0|||-1|-1||-1|-1||1||||||0|\nadd_button|weather_sunny|`oWeather Machine - Sunny``|interface/large/store_buttons/store_buttons5.rttex|`2You Get:`` 1 Weather Machine - Sunny.<CR><CR>`5Description:`` You probably don't need this one... but if you ever have a desire to turn a sunset or desert world back to normal, grab a Sunny Weather Machine to restore the default Growtopia sky! `5It's a perma-item, is never lost when destroyed.``|0|5|1000|0|||-1|-1||-1|-1||1||||||0|\nadd_button|weather_night|`oWeather Machine - Night``|interface/large/store_buttons/store_buttons5.rttex|`2You Get:`` 1 Weather Machine - Night.<CR><CR>`5Description:`` You might not call it weather, but we do! This will turn the background of your world into a lovely night scene with stars and moon. `5It's a perma-item, is never lost when destroyed.``|0|6|10000|0|||-1|-1||-1|-1||1||||||0|\nadd_button|weather_arid|`oWeather Machine - Arid``|interface/large/store_buttons/store_buttons5.rttex|`2You Get:`` 1 Weather Machine - Arid.<CR><CR>`5Description:`` Want your world to look like a cartoon desert? This will turn the background of your world into a desert scene with all the trimmings. `5It's a perma-item, is never lost when destroyed.``|0|7|10000|0|||-1|-1||-1|-1||1||||||0|\nadd_button|weather_rainy|`oWeather Machine - Rainy City``|interface/large/store_buttons/store_buttons6.rttex|`2You Get:`` 1 Weather Machine - Rainy City.<CR><CR>`5Description:`` This will turn the background of your world into a dark, rainy city scene complete with sound effects. `5It's a perma-item, is never lost when destroyed.``|0|5|10000|0|||-1|-1||-1|-1||1||||||0|\nadd_button|weather_warp|`oWeather Machine - Warp Speed``|interface/large/store_buttons/store_buttons11.rttex|`2You Get:`` 1 Weather Machine - Warp Speed.<CR><CR>`5Description:`` This Weather Machine will launch your world through space at relativistic speeds, which will cause you to age more slowly, as well as see stars flying by rapidly in the background. `5It's a perma-item, is never lost when destroyed.``|0|3|10000|0|||-1|-1||-1|-1||1||||||0|\nadd_button|mars_blast|`oMars Blast``|interface/large/store_buttons/store_buttons6.rttex|`2You Get:`` 1 Mars Blast.<CR><CR>`5Description:`` Blast off to Mars!  This powerful rocket ship will launch you to a new world set up like the surface of Mars, with a special martian sky background, and unique terrain not found elsewhere in the solar system. Mars even has lower gravity than Growtopia normally does! Remember: When using this, you are creating a NEW world by typing in a new name. You can't convert an existing world to Mars, that would be dangerous.|0|7|15000|0|||-1|-1||-1|-1||1||||||0|\nadd_button|undersea_blast|`oUndersea Blast``|interface/large/store_buttons/store_buttons9.rttex|`2You Get:`` 1 Undersea Blast.<CR><CR>`5Description:`` Explore the ocean!  This advanced device will terraform a new world set up like the bottom of the ocean, with a special ocean background, and special blocks like Seaweed, Coral, Jellyfish, Sharks, and maybe a special surprise... Remember, by using this you are creating a NEW world by typing in a new name. You can't convert an existing world to an ocean, that would be dangerous.|0|7|15000|0|||-1|-1||-1|-1||1||||||0|\nadd_button|cave_blast|`oCave Blast``|interface/large/store_buttons/store_buttons15.rttex|`2You Get:`` 1 Cave Blast.<CR><CR>`5Description:`` This explosive device will punch a hole in the ground, giving you a dark cavern to explore. There are even rumors of treasure and the entrance to ancient mines, hidden deep in the caves also all kind for farmables can spread each some minutes in this world... but make sure you bring a World Lock. The blasted world is not locked when it's created, so lock it before somebody shows up! Remember: When using this, you are creating a NEW world by typing in a new name. You can't convert an existing world to a cave, that would be dangerous.|0|2|750000|0|||-1|-1||-1|-1||1||||||0|\nadd_button|weather_stuff|`oWeather Machine - Stuff``|interface/large/store_buttons/store_buttons15.rttex|`2You Get:`` 1 Weather Machine - Stuff.<CR><CR>`5Description:`` This is the most fun weather imaginable - Choose any item from your inventory, adjust some settings, and watch it rain down from the sky! Or up, if you prefer reversing the gravity. `5It's a perma-item, is never lost when destroyed.``|0|6|50000|0|||-1|-1||-1|-1||1||||||0|\nadd_button|weather_jungle|`oWeather Machine - Jungle``|interface/large/store_buttons/store_buttons16.rttex|`2You Get:`` 1 Weather Machine - Jungle.<CR><CR>`5Description:`` This weather machine will turn the background of your world into a steamy jungle. `5It's a perma-item, is never lost when destroyed.``|0|5|20000|0|||-1|-1||-1|-1||1||||||0|\nadd_button|weather_backgd|`oWeather Machine - Background``|interface/large/store_buttons/store_buttons17.rttex|`2You Get:`` 1 Weather Machine - Background.<CR><CR>`5Description:`` This amazing device can scan any Background Block, and will make your entire world look like it's been filled with that block. Also handy for hiding music notes! `5It's a perma-item, is never lost when destroyed.``|0|1|150000|0|||-1|-1||-1|-1||1||||||0|\nadd_button|digital_rain_weather|`oWeather Machine - Digital Rain``|interface/large/store_buttons/store_buttons22.rttex|`2You Get:`` 1 Weather Machine - Digital Rain.<CR><CR>`5Description:`` Take the grow pill, and we'll show you how deep the rabbit hole goes! Splash the scrolling code of creation across the skies of your worlds. They say you learn to understand it after a while... Note: You can only have one of these per world. `5It's a perma-item, is never lost when destroyed.``|0|6|30000|0|||-1|-1||-1|-1||1||||||0|\nadd_button|treasure_blast|`oTreasure Blast``|interface/large/store_buttons/store_buttons26.rttex|`2You Get:`` 1 Treasure Blast.<CR><CR>`5Description:`` Enter a world of snow-capped peaks and long-forgotten mysteries! Riddles and secrets - and a ton of treasure - await those who brave this blast's blocks! Remember, when you use this, it'll create a new world by typing in a new name! No sense in searching for clues to great treasures in well-trod worlds, is there?|0|6|10000|0|||-1|-1||-1|-1||1||||||0|\nadd_button|infinity_weather_machine|`oInfinity Weather Machine``|interface/large/store_buttons/store_buttons32.rttex|`2You Get:`` 1 Infinity Weather Machine.<CR><CR>`5Description:`` Imagine being able to predict the weather?! Well, with the Infinity Weather Machine you can! Add multiple Weather Machines to this machine and have them play on a loop, like a weather mix tape, kind of!|0|3|50000|0|||-1|-1||-1|-1||1||||||0|"));
							ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
							enet_peer_send(peer, 0, packet);
							delete p.data;
						}
						if (cch == "action|buy\nitem|weather\n") {
							GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStoreRequest"), "set_description_text|`2Weather Machines!``  Select the item you'd like more info on, or BACK to go back.\nenable_tabs|1\nadd_tab_button|main_menu|Home|interface/large/btn_shop2.rttex||0|0|0|0||||-1|-1|||0|\nadd_tab_button|locks_menu|Locks And Stuff|interface/large/btn_shop2.rttex||0|1|0|0||||-1|-1|||0|\nadd_tab_button|itempack_menu|Item Packs|interface/large/btn_shop2.rttex||0|3|0|0||||-1|-1|||0|\nadd_tab_button|bigitems_menu|Awesome Items|interface/large/btn_shop2.rttex||0|4|0|0||||-1|-1|||0|\nadd_tab_button|weather_menu|Weather Machines|interface/large/btn_shop2.rttex|Tired of the same sunny sky?  We offer alternatives within...|1|5|0|0||||-1|-1|||0|\nadd_tab_button|token_menu|Growtoken Items|interface/large/btn_shop2.rttex||0|2|0|0||||-1|-1|||0|\nadd_button|vegas_pack|`oVegas Pack``|interface/large/store_buttons/store_buttons4.rttex|`2You Get:`` 10 Neon Lights, 1 Card Block Seed, 1 `#Rare Pink Cadillac`` 4 Flipping Coins, 1 Dice Block, 1 Gamblers Visor, 1 Slot Machine, 1 Roulette Wheel and 1 Showgirl Hat, 1 Showgirl top and 1 Showgirl Leggins.<CR><CR>`5Description:`` What happens in Growtopia stays in Growtopia!|0|5|20000|0|||-1|-1||-1|-1||1||||||0|\nadd_button|farm_pack|`oFarm Pack``|interface/large/store_buttons/store_buttons5.rttex|`2You Get:`` 1 Chicken, 1 Cow, 100 Wooden Platform, 40 Chandelier, 40 Laser Grid, 60 Sugar Cane, 75 Pepper Tree, 1 `#Rare`` `2Dear John Tractor``.<CR><CR>`5Description:`` Put the `2Grow`` in Growtopia with this pack, including a Cow you can milk, a Chicken that lays eggs and a farmer's outfit. Best of all? You get a `#Rare`` `2Dear John Tractor`` you can ride that will mow down trees!|0|0|15000|0|||-1|-1||-1|-1||1||||||0|\nadd_button|science_pack|`oMad Science Kit``|interface/large/store_buttons/store_buttons5.rttex|`2You Get:`` 1 Science Station, 1 Laboratory, 1 LabCoat, 1 Combover Hair, 1 Goggles, 5 Chemical R, 10 Chemical G, 5 Chemical Y, 5 Chemical B, 5 Chemical P and 1 `#Rare`` `2Death Ray``.<CR><CR>`5Description:`` It's SCIENCE! Defy the natural order with a Science Station that produces chemicals, a Laboratory in which to mix them and a full outfit to do so safely! You'll also get a starter pack of assorted chemicals. Mix them up! Special bonus: A `#Rare`` `2Death Ray`` to make your science truly mad!|0|3|5000|0|||-1|-1||-1|-1||1||||||0|\nadd_button|city_pack|`oCity Pack``|interface/large/store_buttons/store_buttons6.rttex|`2You Get:`` 10 Sidewalks, 3 Street Signs, 3 Streetlamps, 10 Gothic Building tiles, 10 Tenement Building tiles, 10 Fire Escapes, 3 Gargoyles, 10 Hedges, 1 Blue Mailbox, 1 Fire Hydrant and A `#Rare`` `2ATM Machine``.<CR><CR>`5Description:`` Life in the big city is rough but a `#Rare`` `2ATM Machine`` that dishes out gems once a day is very nice!|0|0|8000|0|||-1|-1||-1|-1||1||||||0|\nadd_button|west_pack|`oWild West Pack``|interface/large/store_buttons/store_buttons6.rttex|`2You Get:`` 1 Cowboy Hat, 1 Cowboy Boots, 1 War Paint, 1 Face Bandana, 1 Sheriff Vest, 1 Layer Cake Dress, 1 Corset, 1 Kansas Curls, 10 Western Building 1 Saloon Doors, 5 Western Banners, 1 Buffalo, 10 Rustic Fences, 1 Campfire and 1 Parasol.<CR><CR>`5Description:`` Yippee-kai-yay! This pack includes everything you need to have wild time in the wild west! The Campfire plays cowboy music, and the `#Parasol`` lets you drift down slowly. Special bonus: A `#Rare`` `2Six Shooter`` to blast criminals with!|0|2|8000|0|||-1|-1||-1|-1||1||||||0|\nadd_button|firefighter|`oFirefighter Pack``|interface/large/store_buttons/store_buttons14.rttex|`2You Get:`` 1 Yellow Helmet, 1 Yellow Jacket, 1 Yellow Pants, 1 Firemans Boots, 1 Fire Hose, and 1 `#Rare Firehouse``.<CR><CR>`5Description:`` Rescue Growtopians from the fire! Includes a full Yellow Firefighter Outfit, Fire Hose and a `#Rare Firehouse``, which will protect your own world from fires.|0|1|10000|0|||-1|-1||-1|-1||1||||||0|\nadd_button|paintbrush|`oPainter's Pack``|interface/large/store_buttons/store_buttons15.rttex|`2You Get:`` 1 `#Rare Paintbrush`` and 20 Random Colored Paint Buckets.<CR><CR>`5Description:`` Want to paint your world? This pack includes 20 buckets of random paint colors (may include Varnish, to clean up your messes)! You can paint any block in your world different colors to personalize it.|0|1|30000|0|||-1|-1||-1|-1||1||||||0|\nadd_button|paleo_kit|`oPaleontologist's Kit``|interface/large/store_buttons/store_buttons16.rttex|`2You Get:`` 5 Fossil Brushes, 1 Rock Hammer, 1 Rock Chisel, 1 Blue Hardhat and 1 `#Rare Fossil Prep Station``.<CR><CR>`5Description:`` If you want to dig up fossils, this is the kit for you! Includes everything you need! Use the prepstation to get your fossils ready for display.|0|0|20000|0|||-1|-1||-1|-1||1||||||0|"));
							ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
							enet_peer_send(peer, 0, packet);
							delete p.data;
						}
						if (cch == "action|buy\nitem|bigitems\n") {
							GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStoreRequest"), "set_description_text|`2Awesome Items!``  Select the item you'd like more info on, or BACK to go back.\nenable_tabs|1\nadd_tab_button|main_menu|Home|interface/large/btn_shop2.rttex||0|0|0|0||||-1|-1|||0|\nadd_tab_button|locks_menu|Locks And Stuff|interface/large/btn_shop2.rttex||0|1|0|0||||-1|-1|||0|\nadd_tab_button|itempack_menu|Item Packs|interface/large/btn_shop2.rttex||0|3|0|0||||-1|-1|||0|\nadd_tab_button|bigitems_menu|Awesome Items|interface/large/btn_shop2.rttex||1|4|0|0||||-1|-1|||0|\nadd_tab_button|weather_menu|Weather Machines|interface/large/btn_shop2.rttex|Tired of the same sunny sky?  We offer alternatives within...|0|5|0|0||||-1|-1|||0|\nadd_tab_button|token_menu|Growtoken Items|interface/large/btn_shop2.rttex||0|2|0|0||||-1|-1|||0|\nadd_button|5seed|`oSmall Seed Pack``|interface/large/store_buttons/store_buttons.rttex|`2You Get:`` 1 Small Seed Pack.<CR><CR>`5Description:`` Contains one Small Seed Pack. Open it for `$5`` randomly chosen seeds, including 1 rare seed! Who knows what you'll get?!|1|4|100|0|||-1|-1||-1|-1||1||||||0|\nadd_button|ssp_10_pack|`oSmall Seed Pack Collection``|interface/large/store_buttons/store_buttons18.rttex|`2You Get:`` 10 Small Seed Packs.<CR><CR>`5Description:`` Open each one for `$5`` randomly chosen seeds apiece, including 1 rare seed per pack! Who knows what you'll get?!|0|4|1000|0|||-1|-1||-1|-1||1||||||0|\nadd_button|rare_seed|`oRare Seed Pack``|interface/large/store_buttons/store_buttons.rttex|`2You Get:`` 5 Randomly Chosen Rare Seeds.<CR><CR>`5Description:`` Expect some wondrous crops with these!|1|7|1000|0|||-1|-1||-1|-1||1||||||0|\nadd_button|grow_spray|`o5-pack of Grow Spray Fertilizer``|interface/large/store_buttons/store_buttons.rttex|`2You Get:`` 5 Grow Spray Fertilizers.<CR><CR>`5Description:`` Why wait?!  Treat yourself to a `$5-pack`` of amazing `wGrow Spray Fertilizer`` by GrowTech Corp.  Each bottle instantly ages a tree by `$1 hour``.|0|6|400|0|||-1|-1||-1|-1||1||||||0|\nadd_button|deluxe_grow_spray|`oDeluxe Grow Spray``|interface/large/store_buttons/store_buttons11.rttex|`2You Get:`` 1 Deluxe Grow Spray.<CR><CR>`5Description:`` GrowTech's new `$Deluxe`` `wGrow Spray`` instantly ages a tree by `$24 hours`` per bottle! That's somewhere around 25 times as much as regular Grow Spray!|0|2|2000|0|||-1|-1||-1|-1||1||||||0|\nadd_button|bountiful_seed_pack|`oBountiful Seed Pack``|interface/large/store_buttons/store_buttons28.rttex|`2You Get:`` 1 Bountiful Seed Pack.<CR><CR>`5Description:`` Contains `$5`` randomly chosen bountiful seeds, including 1 rare seed! Who knows what you'll get?!|0|4|1000|0|||-1|-1||-1|-1||1||||||0|\nadd_button|surgical_kit|`oSurgical Kit``|interface/large/store_buttons/store_buttons7.rttex|`2You Get:`` 1 `#Rare Heart Monitor``, 1 Hospital Bed, 1 Train-E Bot, 5 of each of the 13 different Surical Tools and 10 Med-a-Checks.<CR><CR>`5Description:`` Get all the tools you need to become Chief of Surgery at Growtopia General Hospital! `#Rare`` Heart Monitor that lets people know when you are online, Hospital Bed that lets you perform surgery on anybody laying (or standing) on it, Med-a-Checks to identify patients with maladies, The Train-E bot to practice on, and 5 each of the thirteen different Surgical Tools you'll need to do that surgery!|0|2|12000|0|||-1|-1||-1|-1||1||||||0|0|\nadd_button|fishin_pack|`oFishin' Pack``|interface/large/store_buttons/store_buttons14.rttex|`2You Get:`` 1 Fishing Rod, 5 Wiggly Worms, 1 Hand Drill, 1 Nuclear Detonator,  1 `#Rare Tackle Box``, 10 Fish Tanks and 1 `#Rare Fish Tank Port`` .<CR><CR>`5Description:`` Relax and sit by the shore... this pack includes a Fishing Rod, Wiggly Worms for bait, Hand Drill, Nuclear Detonator, and a `#Rare`` Tackle Box which provides you with more free bait every two days, Fish Tanks, and a `#Rare`` Fish Tank Port to put the fish you catch into your fish tank!|0|0|10000|0|||-1|-1||-1|-1||1||||||0|\nadd_button|guild_name_changer|`oGuild Name Changer``|interface/large/store_buttons/store_buttons23.rttex|`2You Get:`` 1 Guild Name Changer.<CR><CR>`5Description:`` Fancy a change? Bored of your guild name or made a mistake when creating it? Fear not, you can use up one of these to change your `2Guild's name``! The usual name checks will be initiated to check if your new guild name is valid. `4Only usable by the guild leader!``|0|6|100000|0|||-1|-1||-1|-1||1||||||0|\nadd_button|geiger|`oGeiger Counter``|interface/large/store_buttons/store_buttons12.rttex|`2You Get:`` 1 Geiger Counter.<CR><CR>`5Description:`` With this fantabulous device, you can detect radiation around you. It bleeps red, then yellow, then green as you get closer to the source. Who knows what you might find? `4Not available any other way!``|0|1|25000|0|||-1|-1||-1|-1||1||||||0|\nadd_button|guild_chest_pack|`oGuild Chest Pack``|interface/large/store_buttons/store_buttons19.rttex|`2You Get:`` 10 Guild Chests.<CR><CR>`5Description:`` A 10-pack of Guild Chests! Loaded with guildy goodness - pop a chest open for a surprise item!|0|4|20000|0|||-1|-1||-1|-1||1||||||0|"));
							ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
							enet_peer_send(peer, 0, packet);
							delete p.data;
						}
						if (cch == "action|buy\nitem|transmutation_device\n") {
							auto Price = 25000;
							auto ItemID = 9170;
							ifstream ifsz("save/gemdb/_" + pData->rawName + ".txt");
							string content((std::istreambuf_iterator<char>(ifsz)), (std::istreambuf_iterator<char>()));
							auto gembux = atoi(content.c_str());
							if (gembux >= Price) {
								if (CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && CheckItemExists(peer, ItemID) && CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && !CheckItemExists(peer, ItemID)) {
									Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
									GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You don't have enough space in your inventory that. You may be carrying to many of one of the items you are trying to purchase or you don't have enough free spaces to fit them all in your backpack!"));
									ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
									enet_peer_send(peer, 0, packet);
									delete p.data;
									break;
								}
								gembux -= Price;
								ofstream myfile;
								myfile.open("save/gemdb/_" + pData->rawName + ".txt");
								myfile << gembux;
								myfile.close();
								Player::OnSetBux(peer, gembux, 0);
								bool success = true;
								SaveItemMoreTimes(ItemID, 1, peer, success, "Purchased from store");
								Player::PlayAudio(peer, "audio/piano_nice.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You've purchased `o" + getItemDef(ItemID).name + " `wfor `$" + to_string(Price) + " `wGems.\nYou have `$" + to_string(gembux) + " `wGems left.\n\n`5Received: ``" + getItemDef(ItemID).name + ""));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
							else {
								Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You can't afford `o" + getItemDef(ItemID).name + "``!  You're `$" + to_string(Price - gembux) + "`` Gems short."));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
						}
						if (cch == "action|buy\nitem|nyan_hat\n") {
							auto Price = 25000;
							auto ItemID = 574;
							ifstream ifsz("save/gemdb/_" + pData->rawName + ".txt");
							string content((std::istreambuf_iterator<char>(ifsz)), (std::istreambuf_iterator<char>()));
							auto gembux = atoi(content.c_str());
							if (gembux >= Price) {
								if (CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && CheckItemExists(peer, ItemID) && CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && !CheckItemExists(peer, ItemID)) {
									Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
									GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You don't have enough space in your inventory that. You may be carrying to many of one of the items you are trying to purchase or you don't have enough free spaces to fit them all in your backpack!"));
									ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
									enet_peer_send(peer, 0, packet);
									delete p.data;
									break;
								}
								gembux -= Price;
								ofstream myfile;
								myfile.open("save/gemdb/_" + pData->rawName + ".txt");
								myfile << gembux;
								myfile.close();
								Player::OnSetBux(peer, gembux, 0);
								bool success = true;
								SaveItemMoreTimes(ItemID, 1, peer, success, "Purchased from store");
								Player::PlayAudio(peer, "audio/piano_nice.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You've purchased `o" + getItemDef(ItemID).name + " `wfor `$" + to_string(Price) + " `wGems.\nYou have `$" + to_string(gembux) + " `wGems left.\n\n`5Received: ``" + getItemDef(ItemID).name + ""));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
							else {
								Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You can't afford `o" + getItemDef(ItemID).name + "``!  You're `$" + to_string(Price - gembux) + "`` Gems short."));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
						}
						if (cch == "action|buy\nitem|tiny_horsie\n") {
							auto Price = 25000;
							auto ItemID = 592;
							ifstream ifsz("save/gemdb/_" + pData->rawName + ".txt");
							string content((std::istreambuf_iterator<char>(ifsz)), (std::istreambuf_iterator<char>()));
							auto gembux = atoi(content.c_str());
							if (gembux >= Price) {
								if (CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && CheckItemExists(peer, ItemID) && CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && !CheckItemExists(peer, ItemID)) {
									Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
									GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You don't have enough space in your inventory that. You may be carrying to many of one of the items you are trying to purchase or you don't have enough free spaces to fit them all in your backpack!"));
									ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
									enet_peer_send(peer, 0, packet);
									delete p.data;
									break;
								}
								gembux -= Price;
								ofstream myfile;
								myfile.open("save/gemdb/_" + pData->rawName + ".txt");
								myfile << gembux;
								myfile.close();
								Player::OnSetBux(peer, gembux, 0);
								bool success = true;
								SaveItemMoreTimes(ItemID, 1, peer, success, "Purchased from store");
								Player::PlayAudio(peer, "audio/piano_nice.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You've purchased `o" + getItemDef(ItemID).name + " `wfor `$" + to_string(Price) + " `wGems.\nYou have `$" + to_string(gembux) + " `wGems left.\n\n`5Received: ``" + getItemDef(ItemID).name + ""));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
							else {
								Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You can't afford `o" + getItemDef(ItemID).name + "``!  You're `$" + to_string(Price - gembux) + "`` Gems short."));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
						}
						if (cch == "action|buy\nitem|star_ship\n") {
							auto Price = 25000;
							auto ItemID = 760;
							ifstream ifsz("save/gemdb/_" + pData->rawName + ".txt");
							string content((std::istreambuf_iterator<char>(ifsz)), (std::istreambuf_iterator<char>()));
							auto gembux = atoi(content.c_str());
							if (gembux >= Price) {
								if (CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && CheckItemExists(peer, ItemID) && CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && !CheckItemExists(peer, ItemID)) {
									Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
									GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You don't have enough space in your inventory that. You may be carrying to many of one of the items you are trying to purchase or you don't have enough free spaces to fit them all in your backpack!"));
									ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
									enet_peer_send(peer, 0, packet);
									delete p.data;
									break;
								}
								gembux -= Price;
								ofstream myfile;
								myfile.open("save/gemdb/_" + pData->rawName + ".txt");
								myfile << gembux;
								myfile.close();
								Player::OnSetBux(peer, gembux, 0);
								bool success = true;
								SaveItemMoreTimes(ItemID, 1, peer, success, "Purchased from store");
								Player::PlayAudio(peer, "audio/piano_nice.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You've purchased `o" + getItemDef(ItemID).name + " `wfor `$" + to_string(Price) + " `wGems.\nYou have `$" + to_string(gembux) + " `wGems left.\n\n`5Received: ``" + getItemDef(ItemID).name + ""));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
							else {
								Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You can't afford `o" + getItemDef(ItemID).name + "``!  You're `$" + to_string(Price - gembux) + "`` Gems short."));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
						}
						if (cch == "action|buy\nitem|dragon_hand\n") {
							auto Price = 50000;
							auto ItemID = 900;
							ifstream ifsz("save/gemdb/_" + pData->rawName + ".txt");
							string content((std::istreambuf_iterator<char>(ifsz)), (std::istreambuf_iterator<char>()));
							auto gembux = atoi(content.c_str());
							if (gembux >= Price) {
								if (CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && CheckItemExists(peer, ItemID) && CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && !CheckItemExists(peer, ItemID)) {
									Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
									GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You don't have enough space in your inventory that. You may be carrying to many of one of the items you are trying to purchase or you don't have enough free spaces to fit them all in your backpack!"));
									ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
									enet_peer_send(peer, 0, packet);
									delete p.data;
									break;
								}
								gembux -= Price;
								ofstream myfile;
								myfile.open("save/gemdb/_" + pData->rawName + ".txt");
								myfile << gembux;
								myfile.close();
								Player::OnSetBux(peer, gembux, 0);
								bool success = true;
								SaveItemMoreTimes(ItemID, 1, peer, success, "Purchased from store");
								Player::PlayAudio(peer, "audio/piano_nice.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You've purchased `o" + getItemDef(ItemID).name + " `wfor `$" + to_string(Price) + " `wGems.\nYou have `$" + to_string(gembux) + " `wGems left.\n\n`5Received: ``" + getItemDef(ItemID).name + ""));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
							else {
								Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You can't afford `o" + getItemDef(ItemID).name + "``!  You're `$" + to_string(Price - gembux) + "`` Gems short."));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
						}
						if (cch == "action|buy\nitem|corvette\n") {
							auto Price = 25000;
							auto ItemID = 766;
							ifstream ifsz("save/gemdb/_" + pData->rawName + ".txt");
							string content((std::istreambuf_iterator<char>(ifsz)), (std::istreambuf_iterator<char>()));
							auto gembux = atoi(content.c_str());
							if (gembux >= Price) {
								if (CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && CheckItemExists(peer, ItemID) && CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && !CheckItemExists(peer, ItemID)) {
									Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
									GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You don't have enough space in your inventory that. You may be carrying to many of one of the items you are trying to purchase or you don't have enough free spaces to fit them all in your backpack!"));
									ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
									enet_peer_send(peer, 0, packet);
									delete p.data;
									break;
								}
								gembux -= Price;
								ofstream myfile;
								myfile.open("save/gemdb/_" + pData->rawName + ".txt");
								myfile << gembux;
								myfile.close();
								Player::OnSetBux(peer, gembux, 0);
								bool success = true;
								SaveItemMoreTimes(ItemID, 1, peer, success, "Purchased from store");
								Player::PlayAudio(peer, "audio/piano_nice.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You've purchased `o" + getItemDef(ItemID).name + " `wfor `$" + to_string(Price) + " `wGems.\nYou have `$" + to_string(gembux) + " `wGems left.\n\n`5Received: ``" + getItemDef(ItemID).name + ""));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
							else {
								Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You can't afford `o" + getItemDef(ItemID).name + "``!  You're `$" + to_string(Price - gembux) + "`` Gems short."));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
						}
						if (cch == "action|buy\nitem|stick_horse\n") {
							auto Price = 25000;
							auto ItemID = 1012;
							ifstream ifsz("save/gemdb/_" + pData->rawName + ".txt");
							string content((std::istreambuf_iterator<char>(ifsz)), (std::istreambuf_iterator<char>()));
							auto gembux = atoi(content.c_str());
							if (gembux >= Price) {
								if (CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && CheckItemExists(peer, ItemID) && CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && !CheckItemExists(peer, ItemID)) {
									Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
									GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You don't have enough space in your inventory that. You may be carrying to many of one of the items you are trying to purchase or you don't have enough free spaces to fit them all in your backpack!"));
									ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
									enet_peer_send(peer, 0, packet);
									delete p.data;
									break;
								}
								gembux -= Price;
								ofstream myfile;
								myfile.open("save/gemdb/_" + pData->rawName + ".txt");
								myfile << gembux;
								myfile.close();
								Player::OnSetBux(peer, gembux, 0);
								bool success = true;
								SaveItemMoreTimes(ItemID, 1, peer, success, "Purchased from store");
								Player::PlayAudio(peer, "audio/piano_nice.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You've purchased `o" + getItemDef(ItemID).name + " `wfor `$" + to_string(Price) + " `wGems.\nYou have `$" + to_string(gembux) + " `wGems left.\n\n`5Received: ``" + getItemDef(ItemID).name + ""));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
							else {
								Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You can't afford `o" + getItemDef(ItemID).name + "``!  You're `$" + to_string(Price - gembux) + "`` Gems short."));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
						}
						if (cch == "action|buy\nitem|ambulance\n") {
							auto Price = 25000;
							auto ItemID = 1272;
							ifstream ifsz("save/gemdb/_" + pData->rawName + ".txt");
							string content((std::istreambuf_iterator<char>(ifsz)), (std::istreambuf_iterator<char>()));
							auto gembux = atoi(content.c_str());
							if (gembux >= Price) {
								if (CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && CheckItemExists(peer, ItemID) && CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && !CheckItemExists(peer, ItemID)) {
									Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
									GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You don't have enough space in your inventory that. You may be carrying to many of one of the items you are trying to purchase or you don't have enough free spaces to fit them all in your backpack!"));
									ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
									enet_peer_send(peer, 0, packet);
									delete p.data;
									break;
								}
								gembux -= Price;
								ofstream myfile;
								myfile.open("save/gemdb/_" + pData->rawName + ".txt");
								myfile << gembux;
								myfile.close();
								Player::OnSetBux(peer, gembux, 0);
								bool success = true;
								SaveItemMoreTimes(ItemID, 1, peer, success, "Purchased from store");
								Player::PlayAudio(peer, "audio/piano_nice.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You've purchased `o" + getItemDef(ItemID).name + " `wfor `$" + to_string(Price) + " `wGems.\nYou have `$" + to_string(gembux) + " `wGems left.\n\n`5Received: ``" + getItemDef(ItemID).name + ""));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
							else {
								Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You can't afford `o" + getItemDef(ItemID).name + "``!  You're `$" + to_string(Price - gembux) + "`` Gems short."));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
						}
						if (cch == "action|buy\nitem|raptor\n") {
							auto Price = 25000;
							auto ItemID = 1320;
							ifstream ifsz("save/gemdb/_" + pData->rawName + ".txt");
							string content((std::istreambuf_iterator<char>(ifsz)), (std::istreambuf_iterator<char>()));
							auto gembux = atoi(content.c_str());
							if (gembux >= Price) {
								if (CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && CheckItemExists(peer, ItemID) && CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && !CheckItemExists(peer, ItemID)) {
									Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
									GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You don't have enough space in your inventory that. You may be carrying to many of one of the items you are trying to purchase or you don't have enough free spaces to fit them all in your backpack!"));
									ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
									enet_peer_send(peer, 0, packet);
									delete p.data;
									break;
								}
								gembux -= Price;
								ofstream myfile;
								myfile.open("save/gemdb/_" + pData->rawName + ".txt");
								myfile << gembux;
								myfile.close();
								Player::OnSetBux(peer, gembux, 0);
								bool success = true;
								SaveItemMoreTimes(ItemID, 1, peer, success, "Purchased from store");
								Player::PlayAudio(peer, "audio/piano_nice.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You've purchased `o" + getItemDef(ItemID).name + " `wfor `$" + to_string(Price) + " `wGems.\nYou have `$" + to_string(gembux) + " `wGems left.\n\n`5Received: ``" + getItemDef(ItemID).name + ""));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
							else {
								Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You can't afford `o" + getItemDef(ItemID).name + "``!  You're `$" + to_string(Price - gembux) + "`` Gems short."));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
						}
						if (cch == "action|buy\nitem|owl\n") {
							auto Price = 30000;
							auto ItemID = 1540;
							ifstream ifsz("save/gemdb/_" + pData->rawName + ".txt");
							string content((std::istreambuf_iterator<char>(ifsz)), (std::istreambuf_iterator<char>()));
							auto gembux = atoi(content.c_str());
							if (gembux >= Price) {
								if (CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && CheckItemExists(peer, ItemID) && CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && !CheckItemExists(peer, ItemID)) {
									Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
									GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You don't have enough space in your inventory that. You may be carrying to many of one of the items you are trying to purchase or you don't have enough free spaces to fit them all in your backpack!"));
									ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
									enet_peer_send(peer, 0, packet);
									delete p.data;
									break;
								}
								gembux -= Price;
								ofstream myfile;
								myfile.open("save/gemdb/_" + pData->rawName + ".txt");
								myfile << gembux;
								myfile.close();
								Player::OnSetBux(peer, gembux, 0);
								bool success = true;
								SaveItemMoreTimes(ItemID, 1, peer, success, "Purchased from store");
								Player::PlayAudio(peer, "audio/piano_nice.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You've purchased `o" + getItemDef(ItemID).name + " `wfor `$" + to_string(Price) + " `wGems.\nYou have `$" + to_string(gembux) + " `wGems left.\n\n`5Received: ``" + getItemDef(ItemID).name + ""));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
							else {
								Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You can't afford `o" + getItemDef(ItemID).name + "``!  You're `$" + to_string(Price - gembux) + "`` Gems short."));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
						}
						if (cch == "action|buy\nitem|unicorn\n") {
							auto Price = 50000;
							auto ItemID = 1648;
							ifstream ifsz("save/gemdb/_" + pData->rawName + ".txt");
							string content((std::istreambuf_iterator<char>(ifsz)), (std::istreambuf_iterator<char>()));
							auto gembux = atoi(content.c_str());
							if (gembux >= Price) {
								if (CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && CheckItemExists(peer, ItemID) && CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && !CheckItemExists(peer, ItemID)) {
									Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
									GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You don't have enough space in your inventory that. You may be carrying to many of one of the items you are trying to purchase or you don't have enough free spaces to fit them all in your backpack!"));
									ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
									enet_peer_send(peer, 0, packet);
									delete p.data;
									break;
								}
								gembux -= Price;
								ofstream myfile;
								myfile.open("save/gemdb/_" + pData->rawName + ".txt");
								myfile << gembux;
								myfile.close();
								Player::OnSetBux(peer, gembux, 0);
								bool success = true;
								SaveItemMoreTimes(ItemID, 1, peer, success, "Purchased from store");
								Player::PlayAudio(peer, "audio/piano_nice.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You've purchased `o" + getItemDef(ItemID).name + " `wfor `$" + to_string(Price) + " `wGems.\nYou have `$" + to_string(gembux) + " `wGems left.\n\n`5Received: ``" + getItemDef(ItemID).name + ""));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
							else {
								Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You can't afford `o" + getItemDef(ItemID).name + "``!  You're `$" + to_string(Price - gembux) + "`` Gems short."));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
						}
						if (cch == "action|buy\nitem|starboard\n") {
							auto Price = 30000;
							auto ItemID = 1740;
							ifstream ifsz("save/gemdb/_" + pData->rawName + ".txt");
							string content((std::istreambuf_iterator<char>(ifsz)), (std::istreambuf_iterator<char>()));
							auto gembux = atoi(content.c_str());
							if (gembux >= Price) {
								if (CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && CheckItemExists(peer, ItemID) && CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && !CheckItemExists(peer, ItemID)) {
									Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
									GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You don't have enough space in your inventory that. You may be carrying to many of one of the items you are trying to purchase or you don't have enough free spaces to fit them all in your backpack!"));
									ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
									enet_peer_send(peer, 0, packet);
									delete p.data;
									break;
								}
								gembux -= Price;
								ofstream myfile;
								myfile.open("save/gemdb/_" + pData->rawName + ".txt");
								myfile << gembux;
								myfile.close();
								Player::OnSetBux(peer, gembux, 0);
								bool success = true;
								SaveItemMoreTimes(ItemID, 1, peer, success, "Purchased from store");
								Player::PlayAudio(peer, "audio/piano_nice.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You've purchased `o" + getItemDef(ItemID).name + " `wfor `$" + to_string(Price) + " `wGems.\nYou have `$" + to_string(gembux) + " `wGems left.\n\n`5Received: ``" + getItemDef(ItemID).name + ""));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
							else {
								Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You can't afford `o" + getItemDef(ItemID).name + "``!  You're `$" + to_string(Price - gembux) + "`` Gems short."));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
						}
						if (cch == "action|buy\nitem|motorcycle\n") {
							auto Price = 50000;
							auto ItemID = 1950;
							ifstream ifsz("save/gemdb/_" + pData->rawName + ".txt");
							string content((std::istreambuf_iterator<char>(ifsz)), (std::istreambuf_iterator<char>()));
							auto gembux = atoi(content.c_str());
							if (gembux >= Price) {
								if (CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && CheckItemExists(peer, ItemID) && CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && !CheckItemExists(peer, ItemID)) {
									Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
									GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You don't have enough space in your inventory that. You may be carrying to many of one of the items you are trying to purchase or you don't have enough free spaces to fit them all in your backpack!"));
									ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
									enet_peer_send(peer, 0, packet);
									delete p.data;
									break;
								}
								gembux -= Price;
								ofstream myfile;
								myfile.open("save/gemdb/_" + pData->rawName + ".txt");
								myfile << gembux;
								myfile.close();
								Player::OnSetBux(peer, gembux, 0);
								bool success = true;
								SaveItemMoreTimes(ItemID, 1, peer, success, "Purchased from store");
								Player::PlayAudio(peer, "audio/piano_nice.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You've purchased `o" + getItemDef(ItemID).name + " `wfor `$" + to_string(Price) + " `wGems.\nYou have `$" + to_string(gembux) + " `wGems left.\n\n`5Received: ``" + getItemDef(ItemID).name + ""));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
							else {
								Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You can't afford `o" + getItemDef(ItemID).name + "``!  You're `$" + to_string(Price - gembux) + "`` Gems short."));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
						}
						if (cch == "action|buy\nitem|monkey_on_back\n") {
							auto Price = 50000;
							auto ItemID = 2900;
							ifstream ifsz("save/gemdb/_" + pData->rawName + ".txt");
							string content((std::istreambuf_iterator<char>(ifsz)), (std::istreambuf_iterator<char>()));
							auto gembux = atoi(content.c_str());
							if (gembux >= Price) {
								if (CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && CheckItemExists(peer, ItemID) && CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && !CheckItemExists(peer, ItemID)) {
									Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
									GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You don't have enough space in your inventory that. You may be carrying to many of one of the items you are trying to purchase or you don't have enough free spaces to fit them all in your backpack!"));
									ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
									enet_peer_send(peer, 0, packet);
									delete p.data;
									break;
								}
								gembux -= Price;
								ofstream myfile;
								myfile.open("save/gemdb/_" + pData->rawName + ".txt");
								myfile << gembux;
								myfile.close();
								Player::OnSetBux(peer, gembux, 0);
								bool success = true;
								SaveItemMoreTimes(ItemID, 1, peer, success, "Purchased from store");
								Player::PlayAudio(peer, "audio/piano_nice.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You've purchased `o" + getItemDef(ItemID).name + " `wfor `$" + to_string(Price) + " `wGems.\nYou have `$" + to_string(gembux) + " `wGems left.\n\n`5Received: ``" + getItemDef(ItemID).name + ""));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
							else {
								Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You can't afford `o" + getItemDef(ItemID).name + "``!  You're `$" + to_string(Price - gembux) + "`` Gems short."));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
						}
						if (cch == "action|buy\nitem|carrot_sword\n") {
							auto Price = 15000;
							auto ItemID = 2908;
							ifstream ifsz("save/gemdb/_" + pData->rawName + ".txt");
							string content((std::istreambuf_iterator<char>(ifsz)), (std::istreambuf_iterator<char>()));
							auto gembux = atoi(content.c_str());
							if (gembux >= Price) {
								if (CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && CheckItemExists(peer, ItemID) && CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && !CheckItemExists(peer, ItemID)) {
									Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
									GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You don't have enough space in your inventory that. You may be carrying to many of one of the items you are trying to purchase or you don't have enough free spaces to fit them all in your backpack!"));
									ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
									enet_peer_send(peer, 0, packet);
									delete p.data;
									break;
								}
								gembux -= Price;
								ofstream myfile;
								myfile.open("save/gemdb/_" + pData->rawName + ".txt");
								myfile << gembux;
								myfile.close();
								Player::OnSetBux(peer, gembux, 0);
								bool success = true;
								SaveItemMoreTimes(ItemID, 1, peer, success, "Purchased from store");
								Player::PlayAudio(peer, "audio/piano_nice.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You've purchased `o" + getItemDef(ItemID).name + " `wfor `$" + to_string(Price) + " `wGems.\nYou have `$" + to_string(gembux) + " `wGems left.\n\n`5Received: ``" + getItemDef(ItemID).name + ""));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
							else {
								Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You can't afford `o" + getItemDef(ItemID).name + "``!  You're `$" + to_string(Price - gembux) + "`` Gems short."));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
						}
						if (cch == "action|buy\nitem|red_bicycle\n") {
							auto Price = 30000;
							auto ItemID = 2974;
							ifstream ifsz("save/gemdb/_" + pData->rawName + ".txt");
							string content((std::istreambuf_iterator<char>(ifsz)), (std::istreambuf_iterator<char>()));
							auto gembux = atoi(content.c_str());
							if (gembux >= Price) {
								if (CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && CheckItemExists(peer, ItemID) && CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && !CheckItemExists(peer, ItemID)) {
									Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
									GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You don't have enough space in your inventory that. You may be carrying to many of one of the items you are trying to purchase or you don't have enough free spaces to fit them all in your backpack!"));
									ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
									enet_peer_send(peer, 0, packet);
									delete p.data;
									break;
								}
								gembux -= Price;
								ofstream myfile;
								myfile.open("save/gemdb/_" + pData->rawName + ".txt");
								myfile << gembux;
								myfile.close();
								Player::OnSetBux(peer, gembux, 0);
								bool success = true;
								SaveItemMoreTimes(ItemID, 1, peer, success, "Purchased from store");
								Player::PlayAudio(peer, "audio/piano_nice.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You've purchased `o" + getItemDef(ItemID).name + " `wfor `$" + to_string(Price) + " `wGems.\nYou have `$" + to_string(gembux) + " `wGems left.\n\n`5Received: ``" + getItemDef(ItemID).name + ""));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
							else {
								Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You can't afford `o" + getItemDef(ItemID).name + "``!  You're `$" + to_string(Price - gembux) + "`` Gems short."));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
						}
						if (cch == "action|buy\nitem|fire_truck\n") {
							auto Price = 50000;
							auto ItemID = 3068;
							ifstream ifsz("save/gemdb/_" + pData->rawName + ".txt");
							string content((std::istreambuf_iterator<char>(ifsz)), (std::istreambuf_iterator<char>()));
							auto gembux = atoi(content.c_str());
							if (gembux >= Price) {
								if (CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && CheckItemExists(peer, ItemID) && CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && !CheckItemExists(peer, ItemID)) {
									Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
									GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You don't have enough space in your inventory that. You may be carrying to many of one of the items you are trying to purchase or you don't have enough free spaces to fit them all in your backpack!"));
									ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
									enet_peer_send(peer, 0, packet);
									delete p.data;
									break;
								}
								gembux -= Price;
								ofstream myfile;
								myfile.open("save/gemdb/_" + pData->rawName + ".txt");
								myfile << gembux;
								myfile.close();
								Player::OnSetBux(peer, gembux, 0);
								bool success = true;
								SaveItemMoreTimes(ItemID, 1, peer, success, "Purchased from store");
								Player::PlayAudio(peer, "audio/piano_nice.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You've purchased `o" + getItemDef(ItemID).name + " `wfor `$" + to_string(Price) + " `wGems.\nYou have `$" + to_string(gembux) + " `wGems left.\n\n`5Received: ``" + getItemDef(ItemID).name + ""));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
							else {
								Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You can't afford `o" + getItemDef(ItemID).name + "``!  You're `$" + to_string(Price - gembux) + "`` Gems short."));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
						}
						if (cch == "action|buy\nitem|pet_slime\n") {
							auto Price = 100000;
							auto ItemID = 3166;
							ifstream ifsz("save/gemdb/_" + pData->rawName + ".txt");
							string content((std::istreambuf_iterator<char>(ifsz)), (std::istreambuf_iterator<char>()));
							auto gembux = atoi(content.c_str());
							if (gembux >= Price) {
								if (CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && CheckItemExists(peer, ItemID) && CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && !CheckItemExists(peer, ItemID)) {
									Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
									GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You don't have enough space in your inventory that. You may be carrying to many of one of the items you are trying to purchase or you don't have enough free spaces to fit them all in your backpack!"));
									ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
									enet_peer_send(peer, 0, packet);
									delete p.data;
									break;
								}
								gembux -= Price;
								ofstream myfile;
								myfile.open("save/gemdb/_" + pData->rawName + ".txt");
								myfile << gembux;
								myfile.close();
								Player::OnSetBux(peer, gembux, 0);
								bool success = true;
								SaveItemMoreTimes(ItemID, 1, peer, success, "Purchased from store");
								Player::PlayAudio(peer, "audio/piano_nice.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You've purchased `o" + getItemDef(ItemID).name + " `wfor `$" + to_string(Price) + " `wGems.\nYou have `$" + to_string(gembux) + " `wGems left.\n\n`5Received: ``" + getItemDef(ItemID).name + ""));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
							else {
								Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You can't afford `o" + getItemDef(ItemID).name + "``!  You're `$" + to_string(Price - gembux) + "`` Gems short."));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
						}
						if (cch == "action|buy\nitem|dabstep_shoes\n") {
							auto Price = 30000;
							auto ItemID = 6780;
							ifstream ifsz("save/gemdb/_" + pData->rawName + ".txt");
							string content((std::istreambuf_iterator<char>(ifsz)), (std::istreambuf_iterator<char>()));
							auto gembux = atoi(content.c_str());
							if (gembux >= Price) {
								if (CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && CheckItemExists(peer, ItemID) && CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && !CheckItemExists(peer, ItemID)) {
									Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
									GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You don't have enough space in your inventory that. You may be carrying to many of one of the items you are trying to purchase or you don't have enough free spaces to fit them all in your backpack!"));
									ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
									enet_peer_send(peer, 0, packet);
									delete p.data;
									break;
								}
								gembux -= Price;
								ofstream myfile;
								myfile.open("save/gemdb/_" + pData->rawName + ".txt");
								myfile << gembux;
								myfile.close();
								Player::OnSetBux(peer, gembux, 0);
								bool success = true;
								SaveItemMoreTimes(ItemID, 1, peer, success, "Purchased from store");
								Player::PlayAudio(peer, "audio/piano_nice.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You've purchased `o" + getItemDef(ItemID).name + " `wfor `$" + to_string(Price) + " `wGems.\nYou have `$" + to_string(gembux) + " `wGems left.\n\n`5Received: ``" + getItemDef(ItemID).name + ""));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
							else {
								Player::PlayAudio(peer, "audio/bleep_fail.wav", 0);
								GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnStorePurchaseResult"), "You can't afford `o" + getItemDef(ItemID).name + "``!  You're `$" + to_string(Price - gembux) + "`` Gems short."));
								ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
								delete p.data;
							}
						}
					}
					else if (cch.find("action|info") == 0) {
						try {
							std::stringstream ss(cch);
							std::string to;
							int id = -1;
							int count = -1;
							while (std::getline(ss, to, '\n')) {
								vector<string> infoDat = explode("|", to);
								if (infoDat.size() == 3) {
									if (infoDat.at(1) == "itemID") id = atoi(infoDat.at(2).c_str());
									if (infoDat.at(1) == "count") count = atoi(infoDat.at(2).c_str());
								}
							}
							if (id == -1 || count == -1 || itemDefs.size() < id || id < 0) break;
							string properties = "\nadd_textbox|";
							if (id == 18) {
								properties += "<CR>`oYou've punched `w" + to_string(pData->totalpunch) + " `otimes.``";
							}
							ItemDefinition itemDef = getItemDef(id);
							if (itemDef.properties & Property_Untradable) properties += "<CR>`1This item cannot be dropped or traded.`` ";
							if (itemDef.properties & Property_Wrenchable) properties += "<CR>`1This item has special properties you can adjust with the Wrench.`` ";
							if (itemDef.properties & Property_NoSeed) properties += "<CR>`1This item never drops any seeds.`` ";
							if (itemDef.properties & Property_Permanent) properties += "<CR>`1This item can't be destroyed - smashing it will return it to your backpack if you have room!`` ";
							if (properties != "\nadd_textbox|") properties += "|left|";
							else properties = "";
							Player::OnDialogRequest(peer, "set_default_color|`o\nadd_label_with_ele_icon|big|`wAbout " + itemDef.name + "``|left|" + std::to_string(id) + "|3|\nadd_spacer|small|\nadd_textbox|" + itemDef.description + "|left|" + properties + "|\nadd_spacer|small|\nend_dialog|continue||OK|");
						}
						catch (const std::out_of_range& e) {
							std::cout << e.what() << std::endl;
						}
						break;
					}
					else if (cch.find("action|dialog_return") == 0) {
						if (pData->isIn == false || pData->wrenchsession < 0 || pData->wrenchsession > 6000 || world == nullptr || pData->currentWorld == "EXIT" || serverIsFrozen) break;
						if (pData->wrenchsession == 0) {
							if (pData->lastPunchX < 0 || pData->lastPunchY < 0 || pData->lastPunchX > 6000 || pData->lastPunchY > 10000) {
								if (pData->lastPunchX != -1 && pData->lastPunchY != -1) {
									ColoredText("Invalid structure handler, x was " + to_string(pData->lastPunchX) + " and y was " + to_string(pData->lastPunchY), "yellow");
									break;
								}
							}
						}
						else if (pData->wrenchsession < 0 || pData->wrenchsession > 6000) {
							ColoredText("Invalid session handler, wrenchsession was " + to_string(pData->wrenchsession) + "", "yellow");
							break;
						}
						std::stringstream ss(cch);
						std::string to;
						int b_days = 0, b_hours = 0, b_minutes = 0, givesafeitemItemIDInt = 0, givesafeitemCountInt = 0, safe_withdrawConfirmCountInt = 0, safe_withdrawConfirmAposInt = 0, givedonationboxitemCountInt = 0, givedonationboxitemItemIDInt = 0, setprice = 0, chk_peritem = 0, chk_perlock = 0, stockitem = 0, buyprice = 0;
						string vend_set_price = "0", per_item = "0", vverify = "0", vbuycount = "0", vexpectprice = "0", vexpectitem = "0", stuff_gravity = "", stuff_invert = "", stuff_spin = "", stuffITEM = "";
						bool triggersell = false, confirm_create = false, registerguild = false, isPassDoorDialog = false, isGuildDialog = false, isRegisterDialog = false, isDropDialog = false, isAutoBreakDialog = false, isGrowScanDialog = false, isAutoPlaceDialog = false, isMarketDialog = false, isTitleDialog = false, isRiftDialog = false, isMannequinEdit = false, isTrashDialog = false, isMinerDialog = false, trashitem2 = false, isStuffDialog = false, isBackgroundDialog = false, isMagDialog = false, isMagCheckDialog = false, isLockDialog = false, isEntranceDialog = false, isshowlocation = false, MagUpdateDialog = false, isLargeDialog = false, wizardDialog = false;
						bool isThermoDialog = false;
						bool createGuildDialog = false, isTinyDialog = false;
						bool isCaveDialog = false;
						bool isTeleDialog = false;
						bool isHeatwaveDialog = false;
						bool isEditDoorDialog = false;
						bool isSongDialog = false;
						bool isValentineDialog = false;
						bool worldcategoryDialog = false;
						bool signEditor = false;
						bool isgivedonationboxitemConfirm = false;
						bool isFindDialog = false;
						bool isWrenchDialog = false;
						bool isRolesDialog = false;
						bool isTradeDialog = false;
						bool isaddItemToDonationBox = false;
						bool isboxlvl1AddItemDialog = false;
						bool isboxlvl2AddItemDialog = false;
						bool isboxlvl3AddItemDialog = false;
						bool changepasswordDialog = false;
						bool isSafeAddItemDialog = false;
						bool issafedepositconfirmDialog = false;
						bool issafewithdrawConfirmDialog = false;
						bool notebook_edit = false;
						bool title_save = false;
						bool vending = false;
						bool vending_buy = false;
						bool SurgeryDialog = false;
						bool SocialPortalDialog = false;
						bool tradeConfirmDialog = false;
						bool banPanelDialog = false;
						bool cursePanelDialog = false;
						bool mutePanelDialog = false;
						bool isBoardcastDialog = false;
						string boardcasttext = "";
						string guild_name;
						string guild_statement;
						string guild_mascot_bg;
						string guild_mascot_fg;
						string addItemToDonationBox = "";
						string stuffitem = "";
						string magplantitem = "";
						string emeraldbfg = "";
						string gravitystr = "";
						string btn = "";
						string register_username = "", register_password = "", register_password_verify = "", register_email = "";
						string dropitemcount = "";
						string trashitemcount = "";
						string door_destination_world = "", door_destination_id = "", door_name = "", door_id = "";
						string entranceresult = "";
						string playerNetId = "";
						string muted = "";
						string publicremote = "";
						string telefonas = "";
						string uranusnames = "";
						string tinynames = "";
						string largenames = "";
						string thermonames = "";
						string message321 = "";
						string givedonationboxitemCount = "";
						string boxlvl1AddItemDialog = "";
						string boxlvl2AddItemDialog = "";
						string boxlvl3AddItemDialog = "";
						string SafeAddItemDialog = "";
						string safedepositcount = "";
						string safedepositcoord = "";
						string safedeposititemID = "";
						string safe_withdrawConfirmCount = "";
						string h_r = "";
						string h_g = "";
						string h_b = "";
						string safe_withdrawConfirmApos = "";
						string worldpublic = "";
						string guild_name_c = "";
						string safe_withdrawConfirmCoord = "";
						string givedonationboxitemCoord = "";
						string givedonationboxitemItemID = "";
						string givedonationboxitem = "";
						string pass_door_password = "";
						string givedonationboxitemNote = "";
						string title_name = "";
						string personal_note = "";
						string current_password = "";
						string new_password = "";
						string new_password_confirm = "";
						try {
							while (std::getline(ss, to, '\n')) {
								vector<string> infoDat = explode("|", to);
								if (infoDat.size() == 2) {
									if (infoDat.at(0) == "buttonClicked") btn = infoDat.at(1);
									if (pData->trade && infoDat.at(0) == "dialog_return" && infoDat.at(1) != "trade_add") {
										end_trade(peer);
										break;
									}
									if (pData->trade && infoDat.at(0) == "dialog_return" && infoDat.at(1) != "trade_confirm") {
										end_trade(peer);
										break;
									}
									if (pData->haveGrowId == false && infoDat.at(0) == "dialog_name" && infoDat.at(1) != "register") break;
									if (pData->haveGrowId == false && btn != "") break;
									if (infoDat.at(0) == "dialog_name" && infoDat.at(1) == "worldcategory") worldcategoryDialog = true;
									if (infoDat[0] == "dialog_name" && infoDat[1] == "boardcastDialog") isBoardcastDialog = true;
									if (infoDat.at(0) == "dialog_name" && infoDat.at(1) == "vending") vending = true;
									if (infoDat.at(0) == "dialog_name" && infoDat.at(1) == "vending_buy") vending_buy = true;
									if (infoDat.at(0) == "dialog_name" && infoDat.at(1) == "givedonationboxitemConfirm") isgivedonationboxitemConfirm = true;
									if (infoDat.at(0) == "dialog_name" && infoDat.at(1) == "notebook_edit") notebook_edit = true;
									if (infoDat.at(0) == "dialog_name" && infoDat.at(1) == "title_save") title_save = true;
									if (infoDat.at(0) == "dialog_name" && infoDat.at(1) == "gateway_edit") isEntranceDialog = true;
									if (infoDat.at(0) == "dialog_name" && infoDat.at(1) == "editsign") signEditor = true;
									if (infoDat.at(0) == "dialog_name" && infoDat.at(1) == "showlocation") isshowlocation = true;
									if (infoDat.at(0) == "dialog_name" && infoDat.at(1) == "lock_edit") isLockDialog = true;
									if (infoDat.at(0) == "dialog_name" && infoDat.at(1) == "magplantcheck") isMagCheckDialog = true;
									if (infoDat.at(0) == "dialog_name" && infoDat.at(1) == "magplantupdate") MagUpdateDialog = true;
									if (infoDat.at(0) == "dialog_name" && infoDat.at(1) == "safe_withdrawConfirm") issafewithdrawConfirmDialog = true;
									if (infoDat.at(0) == "dialog_name" && infoDat.at(1) == "safedepositconfirm") issafedepositconfirmDialog = true;
									if (infoDat.at(0) == "dialog_name" && infoDat.at(1) == "surge") SurgeryDialog = true;
									if (infoDat.at(0).substr(0, 16) == "addDonationItem_") isaddItemToDonationBox = true;
									if (infoDat.at(0).substr(0, 12) == "safedeposit_") isSafeAddItemDialog = true;
									if (infoDat.at(0).substr(0, 15) == "boxlvl1deposit_") isboxlvl1AddItemDialog = true;
									if (infoDat.at(0).substr(0, 15) == "boxlvl2deposit_") isboxlvl2AddItemDialog = true;
									if (infoDat.at(0).substr(0, 15) == "boxlvl3deposit_") isboxlvl3AddItemDialog = true;
									if (infoDat.at(0) == "dialog_name" && infoDat.at(1) == "weatherspcl9") isHeatwaveDialog = true;
									if (infoDat.at(0) == "dialog_name" && infoDat.at(1) == "weatherspcl") isStuffDialog = true;
									if (infoDat.at(0) == "dialog_name" && infoDat.at(1) == "weatherspcl3") isBackgroundDialog = true;
									if (infoDat.at(0) == "magplantitem") isMagDialog = true;
									if (infoDat.at(0) == "dialog_name" && infoDat.at(1) == "mainwrenchpage") isWrenchDialog = true;
									if (infoDat.at(0) == "dialog_name" && infoDat.at(1) == "roleRewardsPage") isRolesDialog = true;
									if (infoDat.at(0) == "dialog_name" && infoDat.at(1) == "dropdialog") isDropDialog = true;
									if (infoDat.at(0) == "dialog_name" && infoDat.at(1) == "autobreak") isAutoBreakDialog = true;
									if (infoDat.at(0) == "dialog_name" && infoDat.at(1) == "autoplace") isAutoPlaceDialog = true;
									if (infoDat.at(0) == "dialog_name" && infoDat.at(1) == "trashdialog") isTrashDialog = true;
									if (infoDat.at(0) == "dialog_name" && infoDat.at(1) == "findid") isFindDialog = true;
									if (infoDat.at(0) == "dialog_name" && infoDat.at(1) == "guildconfirm") isGuildDialog = true;
									if (infoDat.at(0) == "dialog_name" && infoDat.at(1) == "door_edit") isEditDoorDialog = true;
									if (infoDat.at(0) == "dialog_name" && infoDat.at(1) == "register") isRegisterDialog = true;
									if (infoDat.at(0) == "dialog_name" && infoDat.at(1) == "song_edit") isSongDialog = true;
									if (infoDat.at(0) == "dialog_name" && infoDat.at(1) == "mannequin_edit") isMannequinEdit = true;
									if (infoDat.at(0) == "dialog_name" && infoDat.at(1) == "uselargeblast") isLargeDialog = true;
									if (infoDat.at(0) == "dialog_name" && infoDat.at(1) == "usethermoblast") isThermoDialog = true;
									if (infoDat.at(0) == "dialog_name" && infoDat.at(1) == "usetinyblast") isTinyDialog = true;
									if (infoDat.at(0) == "dialog_name" && infoDat.at(1) == "usecaveblast") isCaveDialog = true;
									if (infoDat.at(0) == "dialog_name" && infoDat.at(1) == "usetelephone") isTeleDialog = true;
									if (infoDat.at(0) == "dialog_name" && infoDat.at(1) == "password_reply") isPassDoorDialog = true;
									if (infoDat.at(0) == "dialog_name" && infoDat.at(1) == "statsblock") isGrowScanDialog = true;
									if (infoDat.at(0) == "dialog_name" && infoDat.at(1) == "valentines_quest") isValentineDialog = true;
									if (infoDat.at(0) == "dialog_name" && infoDat.at(1) == "trash_item2") trashitem2 = true;
									if (infoDat.at(0) == "dialog_name" && infoDat.at(1) == "phonecall") isMinerDialog = true;
									if (infoDat.at(0) == "dialog_name" && infoDat.at(1) == "purchase_item") isMarketDialog = true;
									if (infoDat.at(0) == "dialog_name" && infoDat.at(1) == "password_change") changepasswordDialog = true;
									if (infoDat.at(0) == "dialog_name" && infoDat.at(1) == "dialog_rift_cape") isRiftDialog = true;
									if (infoDat.at(0) == "dialog_name" && infoDat.at(1) == "title_edit") isTitleDialog = true;
									if (infoDat.at(0) == "dialog_name" && infoDat.at(1) == "legendary_wizard") wizardDialog = true;
									if (infoDat.at(0) == "dialog_name" && infoDat.at(1) == "trade_confirm") tradeConfirmDialog = true;
									if (infoDat.at(0) == "dialog_name" && infoDat.at(1) == "ban_panel") banPanelDialog = true;
									if (infoDat.at(0) == "dialog_name" && infoDat.at(1) == "curse_panel") cursePanelDialog = true;
									if (infoDat.at(0) == "dialog_name" && infoDat.at(1) == "mute_panel") mutePanelDialog = true;
									if (infoDat.at(0) == "dialog_name" && infoDat.at(1) == "zodiacpets_quest") {
										if (pData->haveGrowId) {
										}
										break;
									}
									if (infoDat.at(0) == "dialog_name" && infoDat.at(1) == "friends_guilds") SocialPortalDialog = true;
									if (infoDat.at(0) == "dialog_name" && infoDat.at(1) == "guildsignup") registerguild = true;
									if (infoDat.at(0) == "dialog_name" && infoDat.at(1) == "store_sell_confirm") {
										if (pData->laststoreselldiamondlocks == 0 && pData->laststoresellworldlocks == 0 || pData->laststoresellid == 0 || pData->laststoresellcount == 0) break;
										int diamondlocks = pData->laststoreselldiamondlocks;
										int worldlocks = pData->laststoresellworldlocks;
										int sell_id = pData->laststoresellid;
										int sell_count = pData->laststoresellcount;
										auto contains = false;
										SearchInventoryItem(peer, sell_id, sell_count, contains);
										if (!contains) {
											Player::OnTalkBubble(peer, pData->netID, "You don't have " + to_string(sell_count) + " of " + getItemDef(sell_id).name + " to sell!", 0, true);
											break;
										}
										RemoveInventoryItem(sell_id, sell_count, peer, true);
										updateplayerset(peer, sell_id);
										if (diamondlocks != 0) {
											auto success = true;
											SaveItemMoreTimes(1796, diamondlocks, peer, success);
										} if (worldlocks != 0) {
											auto success = true;
											SaveItemMoreTimes(242, worldlocks, peer, success);
										}int kaina_wls = diamondlocks * 100 + worldlocks;
										Player::OnConsoleMessage(peer, "`oSold " + to_string(sell_count) + " `o" + getItemDef(sell_id).name + " `ofor `o" + to_string(kaina_wls) + " `oWorld locks");
										Player::OnTalkBubble(peer, pData->netID, "Sold " + to_string(sell_count) + " " + getItemDef(sell_id).name + " for " + to_string(kaina_wls) + " World locks", 0, true);
										if (diamondlocks != 0) SendTradeEffect(peer, 1796, pData->netID, pData->netID, 150);
										if (worldlocks != 0) SendTradeEffect(peer, 242, pData->netID, pData->netID, 150);
										Player::PlayAudio(peer, "audio/cash_register.wav", 0);
										break;
									}if (infoDat.at(0) == "dialog_name" && infoDat.at(1) == "store_buy_confirm") {
										if (pData->laststorebuyprice == 0 || pData->laststorebuyid == 0 || pData->laststorebuycount == 0) break;
										int kaina_wls = pData->laststorebuyprice;
										int item_id = pData->laststorebuyid;
										int kiek_turi_kainoti = pData->laststorebuycount;
										if (item_id == 9498 || item_id == 5154 || item_id == 5140 || item_id == 5138 || item_id == 5136 || item_id == 7382 || item_id == 4762) {
											kiek_turi_kainoti = rand() % 51 + 150;
										}int diamond_lock = 0, world_lock = 0, grazinti_wl = 0;
										int is_viso_worldlock = kaina_wls;
										while (is_viso_worldlock >= 100) {
											is_viso_worldlock -= 100;
											diamond_lock++;
										}
										world_lock = is_viso_worldlock;
										int turi_world_lock = atoi(GetItemCount(peer, 242).c_str());
										int turi_diamond_lock = atoi(GetItemCount(peer, 1796).c_str());
										while (diamond_lock > turi_diamond_lock && turi_world_lock >= 100) {
											turi_diamond_lock++;
											turi_world_lock -= 100;
										} if (diamond_lock > turi_diamond_lock) {
											Player::OnTalkBubble(peer, pData->netID, "You can't afford that many!", 0, true);
											break;
										} while (world_lock > turi_world_lock && turi_diamond_lock >= 1) {
											turi_diamond_lock--;
											turi_world_lock += 100;
										} if (world_lock > turi_world_lock) {
											Player::OnTalkBubble(peer, pData->netID, "You can't afford that many!", 0, true);
											break;
										}
										turi_world_lock -= world_lock;
										turi_diamond_lock -= diamond_lock;
										RemoveInventoryItem(242, atoi(GetItemCount(peer, 242).c_str()), peer, true);
										RemoveInventoryItem(1796, atoi(GetItemCount(peer, 1796).c_str()), peer, true);
										auto success = true;
										SaveItemMoreTimes(242, turi_world_lock, peer, success);
										SaveItemMoreTimes(1796, turi_diamond_lock, peer, success);
										Player::OnConsoleMessage(peer, "`oPurchased " + to_string(kiek_turi_kainoti) + " `o" + getItemDef(item_id).name + " `ofor `o" + to_string(kaina_wls) + " `oWorld locks");
										Player::OnTalkBubble(peer, pData->netID, "Purchased " + to_string(kiek_turi_kainoti) + " " + getItemDef(item_id).name + " for " + to_string(kaina_wls) + " World locks", 0, true);
										SendTradeEffect(peer, item_id, pData->netID, pData->netID, 150);
										Player::PlayAudio(peer, "audio/cash_register.wav", 0);
										SaveItemMoreTimes(item_id, kiek_turi_kainoti, peer, success);
										try {
											int kiek_pirko = 0;
											ifstream infile("config+/price_data.txt");
											for (string line; getline(infile, line);) {
												if (line.length() > 3 && line.at(0) != '/' && line.at(1) != '/') {
													auto ex = explode("|", line);
													if (ex.at(0) == to_string(item_id)) {
														kiek_pirko = atoi(ex.at(2).c_str());
														break;
													}
												}
											}
											infile.close();
											string strReplace = to_string(item_id) + "|" + to_string(kiek_turi_kainoti) + "|" + to_string(kiek_pirko);
											kiek_pirko += kiek_turi_kainoti;
											string strNew = to_string(item_id) + "|" + to_string(kiek_turi_kainoti) + "|" + to_string(kiek_pirko);
											ifstream filein("config+/price_data.txt");
											ofstream fileout("config+/price_data_backup.txt");
											if (!filein || !fileout) {
												ColoredText("Filesystem Exception #39 Failed to update file", "red");
												return 1;
											}
											string strTemp;
											while (filein >> strTemp) {
												if (strTemp == strReplace) {
													strTemp = strNew;
												}
												strTemp += "\n";
												fileout << strTemp;
											}
											filein.close();
											fileout.close();
											char oldname[] = "config+/price_data_backup.txt";
											char newname[] = "config+/price_data.txt";
											remove("config+/price_data.txt");
											if (rename(oldname, newname) != 0) {
												ColoredText("Filesystem Exception #39 Failed to rename file", "red");
											}
											remove("config+/price_data_backup.txt");
										}
										catch (const std::out_of_range& e) {
											std::cout << e.what() << std::endl;
										}
										break;
									}
									if (infoDat.at(0) == "dialog_name" && infoDat.at(1) == "createguild") createGuildDialog = true;
									if (infoDat.at(0) == "dialog_name" && infoDat.at(1) == "trade_add") isTradeDialog = true;
									if (infoDat.at(0) == "dialog_name" && infoDat.at(1) == "abandon_guild") {
										if (pData->guild == "" || !isWorldOwner(peer, world) && !admin(peer)) break;
										try {
											ifstream read_player("save/players/_" + world->owner + ".json");
											if (!read_player.is_open()) {
												break;
											}
											json j;
											read_player >> j;
											read_player.close();
											string guild_name = j["guild"];
											for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
												if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
												if (static_cast<PlayerInfo*>(currentPeer->data)->guild == guild_name) {
													static_cast<PlayerInfo*>(currentPeer->data)->guild = "";
													static_cast<PlayerInfo*>(currentPeer->data)->guildFg = 0;
													static_cast<PlayerInfo*>(currentPeer->data)->guildBg = 0;
													Player::OnTalkBubble(currentPeer, static_cast<PlayerInfo*>(currentPeer->data)->netID, "You was kicked out from " + guild_name + " guild!", 0, true);
													Player::OnConsoleMessage(currentPeer, "`oYou was kicked out from " + guild_name + " `oguild!");
													Player::PlayAudio(currentPeer, "audio/siren.wav", 0);
													for (ENetPeer* currentPeer2 = server->peers; currentPeer2 < &server->peers[server->peerCount]; ++currentPeer2) {
														if (currentPeer2->state != ENET_PEER_STATE_CONNECTED || currentPeer2->data == NULL) continue;
														if (isHere(currentPeer, currentPeer2)) {
															gamepacket_t p(0, static_cast<PlayerInfo*>(currentPeer->data)->netID);
															p.Insert("OnGuildDataChanged");
															p.Insert(0);
															p.Insert(0);
															p.Insert(0);
															p.Insert(0);
															p.CreatePacket(currentPeer2);
														}
													}
												}
											}
											string del_name = guild_name;
											toUpperCase(del_name);
											world->category = "None";
											remove(("save/guilds/_" + del_name + ".json").c_str());
											Player::OnDialogRequest(peer, "set_default_color|`o\nadd_label_with_icon|big|`wGrow Guild ``|left|242|\nadd_textbox|`oYour guild " + guild_name + " was deleted, sorry to see you go :(|left|\nend_dialog|abandon_close|Close||");
										}
										catch (std::exception& e) {
											std::cout << e.what() << std::endl;
											break;
										}
										break;
									}
									if (infoDat.at(0) == "dialog_name" && infoDat.at(1) == "requestcreateguildpage") {
										std::ifstream ifsz("save/gemdb/_" + pData->rawName + ".txt");
										std::string content((std::istreambuf_iterator<char>(ifsz)), (std::istreambuf_iterator<char>()));
										auto gembux = atoi(content.c_str());
										if (gembux < 100000) {
											Player::OnDialogRequest(peer, "set_default_color|`o\nadd_label_with_icon|big|`wGrow Guild ``|left|5814|\nadd_textbox|You need `4" + to_string(100000 - gembux) + " more Gems`` to be able to create the guild!|left|\nend_dialog|missingguildcreationgems|Close||");
											break;
										}
										Player::OnDialogRequest(peer, "set_default_color|`o\nadd_label_with_icon|big|`wGrow Guild Creation ``|left|5814|\nadd_text_input|guildname|Guild Name:||15|\nadd_text_input|guilddesc|Guild Statement:||24|\nadd_spacer|small|\nadd_textbox|Confirm your guild settings by selecting `2Create Guild`` below to create your guild.|left|\nadd_textbox|`8Remember``: A guild can only be created in a world owned by you and locked with a `5World Lock``!|left|\nadd_spacer|small|\nadd_textbox|`4Warning!`` The guild name cannot be changed once you have confirmed the guild settings!|left|\nadd_spacer|small|\nadd_button|createguild|Confirm|noflags|0|0|\nend_dialog|createguild|Close||");
										break;
									}
									if (mutePanelDialog) {
										if (!mod(peer)) break;
										if (infoDat.at(0) == "days") {
											b_days = atoi(infoDat.at(1).c_str());
											pData->last_ban_days = atoi(infoDat.at(1).c_str());
											if (b_days < 0) {
												send_mute_panel(peer, "`4Error occured: Days cannot be negative!");
												break;
											}
										}
										if (infoDat.at(0) == "hours") {
											b_hours = atoi(infoDat.at(1).c_str());
											pData->last_ban_hours = atoi(infoDat.at(1).c_str());
											if (b_hours < 0) {
												send_mute_panel(peer, "`4Error occured: Hours cannot be negative!");
												break;
											}
										}
										if (infoDat.at(0) == "minutes") {
											b_minutes = atoi(infoDat.at(1).c_str());
											pData->last_ban_minutes = atoi(infoDat.at(1).c_str());
											if (b_minutes < 0) {
												send_mute_panel(peer, "`4Error occured: Minutes cannot be negative!");
												break;
											}
										}
										if (infoDat.at(0) == "reason") {
											string reason = infoDat.at(1);
											pData->last_ban_reason = reason;
											if (b_days == 0 && b_hours == 0 && b_minutes == 0) {
												send_mute_panel(peer, "`4Error occured: Ban length is 0!");
												break;
											}
											else if (reason.size() == 0) {
												send_mute_panel(peer, "`4Error occured: You must enter the reason!");
												break;
											}
											else if (reason.size() >= 90) {
												send_mute_panel(peer, "`4Error occured: Reason is too long!");
												break;
											}
											/*success*/
											int konvertuotasInt = (b_minutes * 60) + (b_hours * 60 * 60) + (b_days * 24 * 60 * 60);
											try {
												ifstream read_player("save/players/_" + pData->lastInfo + ".json");
												if (!read_player.is_open()) {
													continue;
												}
												json j;
												read_player >> j;
												read_player.close();
												j["timemuted"] = (GetCurrentTimeInternalSeconds() + (static_cast<long long>(konvertuotasInt)));
												Player::OnTextOverlay(peer, OutputBanTime(konvertuotasInt) + "Mute mod applied to " + pData->lastInfo + "!");
												ofstream write_player("save/players/_" + pData->lastInfo + ".json");
												write_player << j << std::endl;
												write_player.close();
											}
											catch (std::exception& e) {
												std::cout << e.what() << std::endl;
												continue;
											}
											string userdisplay = "";
											for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
												if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
												if (static_cast<PlayerInfo*>(currentPeer->data)->rawName == pData->lastInfo) {
													userdisplay = static_cast<PlayerInfo*>(currentPeer->data)->displayName;
												}
											} if (userdisplay == "") userdisplay = pData->lastInfo;
											for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
												if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
												Player::OnConsoleMessage(currentPeer, "`#** `$The Ancients `ohave used `#Mute `oon `w" + userdisplay + "`o! `#**");
												if (static_cast<PlayerInfo*>(currentPeer->data)->rawName == pData->lastInfo) {
													Player::OnAddNotification(currentPeer, "`wWarning from `4System`w: You've been `4MUTED `wfor " + OutputBanTime(konvertuotasInt) + "", "audio/hub_open.wav", "interface/atomic_button.rttex");
													Player::OnConsoleMessage(currentPeer, "`wWarning from `4System`w: You've been `4MUTED `wfor " + OutputBanTime(konvertuotasInt) + "");
													static_cast<PlayerInfo*>(currentPeer->data)->taped = true;
													static_cast<PlayerInfo*>(currentPeer->data)->isDuctaped = true;
													static_cast<PlayerInfo*>(currentPeer->data)->cantsay = true;
													static_cast<PlayerInfo*>(currentPeer->data)->lastMuted = (GetCurrentTimeInternalSeconds()) + (static_cast<long long>(konvertuotasInt) * 60);
													send_state(currentPeer);
													sendClothes(currentPeer);
												}
											}
										}
									}
									if (isBoardcastDialog)
									{
										if (infoDat[0] == "boardcasttext") boardcasttext = infoDat[1];
									}
									if (isBoardcastDialog)
									{
										int cooldown_sec = 360;
										RemoveInventoryItem(2480, 1, peer, true);
										ENetPeer* currentPeer;
										for (currentPeer = server->peers;
											currentPeer < &server->peers[server->peerCount];
											++currentPeer)
										{
											if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
											ifstream read_player("save/players/_" + static_cast<PlayerInfo*>(currentPeer->data)->rawName + ".json");
											json j;
											read_player >> j;
											read_player.close();
											j["timesb"] = (GetCurrentTimeInternalSeconds() + (static_cast<long long>(cooldown_sec)));
											ofstream write_player("save/players/_" + static_cast<PlayerInfo*>(currentPeer->data)->rawName + ".json");
											write_player << j << std::endl;
											write_player.close();
											if (static_cast<PlayerInfo*>(currentPeer->data)->rawName == static_cast<PlayerInfo*>(currentPeer->data)->rawName)
											{
												static_cast<PlayerInfo*>(currentPeer->data)->isSBcooldown = true;
												static_cast<PlayerInfo*>(currentPeer->data)->lastSB = (GetCurrentTimeInternalSeconds()) + (static_cast<long long>(cooldown_sec));
											}
											string sb_text = boardcasttext;
											string worldname = static_cast<PlayerInfo*>(peer->data)->currentWorld;
											if (jammers)
											{
												for (auto i = 0; i < world->width * world->height; i++)
												{
													if (world->items.at(i).foreground == 226 && world->items.at(i).activated)
													{
														worldname = "`4JAMMED!";
														break;
													}
												}
												GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnConsoleMessage"), "CP:_PL:0_OID:_CT:[SB]_ `#** `#from (`2" + static_cast<PlayerInfo*>(peer->data)->displayName + "`#) in [`o" + worldname + "`#] ** :`o " + sb_text));
												string text = "action|play_sfx\nfile|audio/beep.wav\ndelayMS|0\n";
												BYTE* data = new BYTE[5 + text.length()];
												BYTE zero = 0;
												int type = 3;
												memcpy(data, &type, 4);
												memcpy(data + 4, text.c_str(), text.length());
												memcpy(data + 4 + text.length(), &zero, 1);
												if (!static_cast<PlayerInfo*>(currentPeer->data)->radio) continue;
												if (static_cast<PlayerInfo*>(currentPeer->data)->isIn == false) continue;
												ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
												enet_peer_send(currentPeer, 0, packet);
												ENetPacket* packet2 = enet_packet_create(data, 5 + text.length(), ENET_PACKET_FLAG_RELIABLE);
												enet_peer_send(currentPeer, 0, packet2);
												static_cast<PlayerInfo*>(currentPeer->data)->lastSbbWorld = static_cast<PlayerInfo*>(peer->data)->currentWorld;
												delete[] data;
												delete p.data;
											}
										}
									}
									if (cursePanelDialog) {
										if (!mod(peer)) break;
										if (infoDat.at(0) == "days") {
											b_days = atoi(infoDat.at(1).c_str());
											pData->last_ban_days = atoi(infoDat.at(1).c_str());
											if (b_days < 0) {
												send_curse_panel(peer, "`4Error occured: Days cannot be negative!");
												break;
											}
										}
										if (infoDat.at(0) == "hours") {
											b_hours = atoi(infoDat.at(1).c_str());
											pData->last_ban_hours = atoi(infoDat.at(1).c_str());
											if (b_hours < 0) {
												send_curse_panel(peer, "`4Error occured: Hours cannot be negative!");
												break;
											}
										}
										if (infoDat.at(0) == "minutes") {
											b_minutes = atoi(infoDat.at(1).c_str());
											pData->last_ban_minutes = atoi(infoDat.at(1).c_str());
											if (b_minutes < 0) {
												send_curse_panel(peer, "`4Error occured: Minutes cannot be negative!");
												break;
											}
										}
										if (infoDat.at(0) == "reason") {
											string reason = infoDat.at(1);
											pData->last_ban_reason = reason;
											if (b_days == 0 && b_hours == 0 && b_minutes == 0) {
												send_curse_panel(peer, "`4Error occured: Ban length is 0!");
												break;
											}
											else if (reason.size() == 0) {
												send_curse_panel(peer, "`4Error occured: You must enter the reason!");
												break;
											}
											else if (reason.size() >= 90) {
												send_curse_panel(peer, "`4Error occured: Reason is too long!");
												break;
											}
											int konvertuotasInt = (b_minutes * 60) + (b_hours * 60 * 60) + (b_days * 24 * 60 * 60);
											try {
												ifstream read_player("save/players/_" + pData->lastInfo + ".json");
												if (!read_player.is_open()) {
													continue;
												}
												json j;
												read_player >> j;
												read_player.close();
												j["timecursed"] = (GetCurrentTimeInternalSeconds() + (static_cast<long long>(konvertuotasInt)));
												Player::OnTextOverlay(peer, OutputBanTime(konvertuotasInt) + "Curse mod applied to " + pData->lastInfo + "!");
												ofstream write_player("save/players/_" + pData->lastInfo + ".json");
												write_player << j << std::endl;
												write_player.close();
											}
											catch (std::exception& e) {
												std::cout << e.what() << std::endl;
												continue;
											}
											string userdisplay = "";
											for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
												if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
												if (static_cast<PlayerInfo*>(currentPeer->data)->rawName == pData->lastInfo) {
													userdisplay = static_cast<PlayerInfo*>(currentPeer->data)->displayName;
												}
											} if (userdisplay == "") userdisplay = pData->lastInfo;
											for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
												if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
												Player::OnConsoleMessage(currentPeer, "`#** `$The Ancients `ohave used `#Curse `oon `w" + userdisplay + "`o! `#**");
												if (static_cast<PlayerInfo*>(currentPeer->data)->rawName == pData->lastInfo) {
													Player::OnAddNotification(currentPeer, "`wWarning from `4System`w: You've been `4CURSED `wfor " + OutputBanTime(konvertuotasInt) + "", "audio/hub_open.wav", "interface/atomic_button.rttex");
													Player::OnConsoleMessage(currentPeer, "`wWarning from `4System`w: You've been `4CURSED `wfor " + OutputBanTime(konvertuotasInt) + "");
													static_cast<PlayerInfo*>(currentPeer->data)->isCursed = true;
													static_cast<PlayerInfo*>(currentPeer->data)->lastCursed = (GetCurrentTimeInternalSeconds() + (static_cast<long long>(konvertuotasInt) * 60));
													handle_world(currentPeer, "HELL");
												}
											}
										}
									}
									if (banPanelDialog) {
										if (!mod(peer)) break;
										if (infoDat.at(0) == "days") {
											b_days = atoi(infoDat.at(1).c_str());
											pData->last_ban_days = atoi(infoDat.at(1).c_str());
											if (b_days < 0) {
												send_ban_panel(peer, "`4Error occured: Days cannot be negative!");
												break;
											}
										}
										if (infoDat.at(0) == "hours") {
											b_hours = atoi(infoDat.at(1).c_str());
											pData->last_ban_hours = atoi(infoDat.at(1).c_str());
											if (b_hours < 0) {
												send_ban_panel(peer, "`4Error occured: Hours cannot be negative!");
												break;
											}
										}
										if (infoDat.at(0) == "minutes") {
											b_minutes = atoi(infoDat.at(1).c_str());
											pData->last_ban_minutes = atoi(infoDat.at(1).c_str());
											if (b_minutes < 0) {
												send_ban_panel(peer, "`4Error occured: Minutes cannot be negative!");
												break;
											}
										}
										if (infoDat.at(0) == "reason") {
											string reason = infoDat.at(1);
											pData->last_ban_reason = reason;
											if (b_days == 0 && b_hours == 0 && b_minutes == 0) {
												send_ban_panel(peer, "`4Error occured: Ban length is 0!");
												break;
											}
											else if (reason.size() == 0) {
												send_ban_panel(peer, "`4Error occured: You must enter the reason!");
												break;
											}
											else if (reason.size() >= 90) {
												send_ban_panel(peer, "`4Error occured: Reason is too long!");
												break;
											}
											int konvertuotasInt = (b_minutes * 60) + (b_hours * 60 * 60) + (b_days * 24 * 60 * 60);
											try {
												ifstream read_player("save/players/_" + pData->lastInfo + ".json");
												if (!read_player.is_open()) {
													continue;
												}
												json j;
												read_player >> j;
												read_player.close();
												if (konvertuotasInt >= 63072000) {
													j["isBanned"] = true;
													Player::OnTextOverlay(peer, "Permanent Ban mod applied to " + pData->lastInfo + "!");
												}
												else {
													j["timebanned"] = (GetCurrentTimeInternalSeconds() + (static_cast<long long>(konvertuotasInt)));
													Player::OnTextOverlay(peer, OutputBanTime(konvertuotasInt) + "Ban mod applied to " + pData->lastInfo + "!");
												}
												ofstream write_player("save/players/_" + pData->lastInfo + ".json");
												write_player << j << std::endl;
												write_player.close();
											}
											catch (std::exception& e) {
												std::cout << e.what() << std::endl;
												continue;
											}
											string userdisplay = "";
											for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
												if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
												if (static_cast<PlayerInfo*>(currentPeer->data)->rawName == pData->lastInfo) {
													userdisplay = static_cast<PlayerInfo*>(currentPeer->data)->displayName;
												}
											} if (userdisplay == "") userdisplay = pData->lastInfo;
											for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
												if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
												Player::OnConsoleMessage(currentPeer, "`#** `$The Ancients `ohave used `#Ban `oon `w" + userdisplay + "`o! `#**");
												if (static_cast<PlayerInfo*>(currentPeer->data)->rawName == pData->lastInfo) {
													Player::OnAddNotification(currentPeer, "`wWarning from `4System`w: You've been `4BANNED `wfor " + OutputBanTime(konvertuotasInt) + "", "audio/hub_open.wav", "interface/atomic_button.rttex");
													Player::OnConsoleMessage(currentPeer, "`wWarning from `4System`w: You've been `4BANNED `wfor " + OutputBanTime(konvertuotasInt) + "");
													enet_peer_disconnect_later(currentPeer, 0);
												}
											}
										}
									}
									if (isTradeDialog) {
										if (infoDat.at(0).substr(0, 6) == "count_") {
											int trade_count = atoi(infoDat.at(1).c_str());
											int trade_item = atoi(infoDat.at(0).substr(infoDat.at(0).find("_") + 1).c_str());
											if (trade_item <= 0 || trade_count <= 0) break;
											if (pData->tradeItems.size() >= 4) continue;
											auto contains = false;
											SearchInventoryItem(peer, trade_item, trade_count, contains);
											if (!contains) break;
											int i = 0;
											for (i = 0; i < pData->tradeItems.size(); i++) {
												if (pData->tradeItems.at(i).id == trade_item) {
													pData->tradeItems.erase(pData->tradeItems.begin() + i);
													break;
												}
											}
											TradeItem trdItem = { trade_item, trade_count };
											pData->tradeItems.push_back(trdItem);
											update_trade(peer, pData->trade_netid);
											break;
										}
									}
									if (tradeConfirmDialog) {
										if (infoDat.at(1) == "back") {
											if (pData->trade) end_trade(peer, true);
											break;
										}
										if (infoDat.at(1) == "accept") {
											if (!pData->trade) {
												Player::OnTextOverlay(peer, "The other person left the trade!");
												break;
											}
											pData->trade_confirmed = true;
											for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
												if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
												if (isHere(peer, currentPeer)) {
													if (static_cast<PlayerInfo*>(currentPeer->data)->netID == pData->trade_netid) {
														if (static_cast<PlayerInfo*>(currentPeer->data)->trade_confirmed && pData->trade_confirmed) {
															bool full_inv = false;
															int which_item = 0;
															if (static_cast<PlayerInfo*>(peer->data)->inventory.items.size() + static_cast<PlayerInfo*>(currentPeer->data)->tradeItems.size() >= static_cast<PlayerInfo*>(peer->data)->currentInventorySize) {
																Player::OnTextOverlay(peer, "`w" + static_cast<PlayerInfo*>(peer->data)->displayName + "`w needs more backpack room first!");
																Player::OnTextOverlay(currentPeer, "`w" + static_cast<PlayerInfo*>(peer->data)->displayName + "`w needs more backpack room first!");
																pData->trade_accept = false;
																static_cast<PlayerInfo*>(currentPeer->data)->trade_accept = false;
																UpdateTradeAcceptedStatus(peer);
																UpdateTradeAcceptedStatus(currentPeer);
																break;
															}
															for (auto& f : static_cast<PlayerInfo*>(currentPeer->data)->tradeItems) {
																if (CheckItemMaxed(peer, f.id, f.count) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && CheckItemExists(peer, f.id) && CheckItemMaxed(peer, f.id, f.count) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && !CheckItemExists(peer, f.id)) {
																	full_inv = true;
																	which_item = f.id;
																	break;
																}
															}
															if (full_inv) {
																Player::OnTextOverlay(currentPeer, "`4Oops - " + pData->displayName + " `4is carrying too many " + getItemDef(which_item).name + " and can't fit that many in their backpack.");
																Player::OnTextOverlay(peer, "`4Oops - " + pData->displayName + " `4is carrying too many " + getItemDef(which_item).name + " and can't fit that many in their backpack.");
																break;
															}
															full_inv = false;
															if (static_cast<PlayerInfo*>(currentPeer->data)->inventory.items.size() + static_cast<PlayerInfo*>(peer->data)->tradeItems.size() >= static_cast<PlayerInfo*>(currentPeer->data)->currentInventorySize) {
																Player::OnTextOverlay(peer, "`w" + static_cast<PlayerInfo*>(currentPeer->data)->displayName + "`w needs more backpack room first!");
																Player::OnTextOverlay(currentPeer, "`w" + static_cast<PlayerInfo*>(currentPeer->data)->displayName + "`w needs more backpack room first!");
																pData->trade_accept = false;
																static_cast<PlayerInfo*>(currentPeer->data)->trade_accept = false;
																UpdateTradeAcceptedStatus(peer);
																UpdateTradeAcceptedStatus(currentPeer);
																break;
															}
															for (auto& f : static_cast<PlayerInfo*>(peer->data)->tradeItems) {
																if (CheckItemMaxed(currentPeer, f.id, f.count) || static_cast<PlayerInfo*>(currentPeer->data)->inventory.items.size() + 1 >= static_cast<PlayerInfo*>(currentPeer->data)->currentInventorySize && CheckItemExists(currentPeer, f.id) && CheckItemMaxed(currentPeer, f.id, f.count) || static_cast<PlayerInfo*>(currentPeer->data)->inventory.items.size() + 1 >= static_cast<PlayerInfo*>(currentPeer->data)->currentInventorySize && !CheckItemExists(currentPeer, f.id)) {
																	full_inv = true;
																	which_item = f.id;
																	break;
																}
															}
															if (full_inv) {
																Player::OnTextOverlay(peer, "`4Oops - " + static_cast<PlayerInfo*>(currentPeer->data)->displayName + " `4is carrying too many " + getItemDef(which_item).name + " and can't fit that many in their backpack.");
																Player::OnTextOverlay(currentPeer, "`4Oops - " + static_cast<PlayerInfo*>(currentPeer->data)->displayName + " `4is carrying too many " + getItemDef(which_item).name + " and can't fit that many in their backpack.");
																break;
															}
															for (ENetPeer* currentPeer2 = server->peers; currentPeer2 < &server->peers[server->peerCount]; ++currentPeer2) {
																if (currentPeer2->state != ENET_PEER_STATE_CONNECTED) continue;
																if (isHere(peer, currentPeer2)) {
																	Player::OnConsoleMessage(currentPeer2, "`1" + static_cast<PlayerInfo*>(currentPeer->data)->displayName + " `1traded " + FormatTradeItemsForMessage(static_cast<PlayerInfo*>(currentPeer->data)->tradeItems) + " to " + pData->displayName + ".``");
																	Player::OnConsoleMessage(currentPeer2, "`1" + pData->displayName + " `1traded " + FormatTradeItemsForMessage(pData->tradeItems) + " to " + static_cast<PlayerInfo*>(currentPeer->data)->displayName + ".``");
																	Player::PlayAudio(currentPeer2, "audio/keypad_hit.wav", 0);
																}
															}
															int duration = 500;
															bool illegal_trade = false;
															for (auto& f : static_cast<PlayerInfo*>(currentPeer->data)->tradeItems) {
																auto contains = false;
																SearchInventoryItem(currentPeer, f.id, f.count, contains);
																if (!contains) {
																	illegal_trade = true;
																	break;
																}
																RemoveInventoryItem(f.id, f.count, currentPeer, true);
																if (f.id == 1424) { /*worldtrade*/
																	for (ENetPeer* currentPeer2 = server->peers; currentPeer2 < &server->peers[server->peerCount]; ++currentPeer2) {
																		if (currentPeer2->state != ENET_PEER_STATE_CONNECTED) continue;
																		if (isHere(peer, currentPeer2)) {
																			Player::OnTalkBubble(currentPeer2, pData->netID, "`5[```w" + world->name + "`` has been `$World Locked`` by " + static_cast<PlayerInfo*>(peer->data)->displayName + "`5]``", 0, false);
																			Player::OnConsoleMessage(currentPeer2, "`5[```w" + world->name + "`` has been `$World Locked`` by " + static_cast<PlayerInfo*>(peer->data)->displayName + "`5]``");
																			Player::PlayAudio(currentPeer2, "audio/use_lock.wav", 0);
																		}
																	}
																	static_cast<PlayerInfo*>(currentPeer->data)->worldsowned.erase(std::remove(static_cast<PlayerInfo*>(currentPeer->data)->worldsowned.begin(), static_cast<PlayerInfo*>(currentPeer->data)->worldsowned.end(), static_cast<PlayerInfo*>(currentPeer->data)->currentWorld), static_cast<PlayerInfo*>(currentPeer->data)->worldsowned.end());
																	world->owner = pData->rawName;
																	pData->worldsowned.push_back(pData->currentWorld);
																	if (static_cast<PlayerInfo*>(currentPeer->data)->NickPrefix == "") updateworldremove(currentPeer);
																	if (pData->displayName.find("`") == string::npos) {
																		pData->displayName = "`2" + pData->displayName;
																		Player::OnNameChanged(peer, pData->netID, pData->displayName);
																	}
																	for (int i = 0; i < world->width * world->height; i++) {
																		if (getItemDef(world->items.at(i).foreground).blockType == BlockTypes::LOCK) {
																			if (world->items.at(i).foreground == 202 || world->items.at(i).foreground == 204 || world->items.at(i).foreground == 206 || world->items.at(i).foreground == 4994) continue;
																			PlayerMoving moving{};
																			moving.packetType = 0x3;
																			moving.characterState = 0x0;
																			moving.x = i % world->width;
																			moving.y = i / world->width;
																			moving.punchX = i % world->width;
																			moving.punchY = i / world->width;
																			moving.XSpeed = 0;
																			moving.YSpeed = 0;
																			moving.netID = -1;
																			moving.plantingTree = world->items.at(i).foreground;
																			SendPacketRaw(4, packPlayerMoving(&moving), 56, nullptr, currentPeer, ENET_PACKET_FLAG_RELIABLE);
																			send_tile_data(peer, i % world->width, i / world->width, 0x10, world->items.at(i).foreground, world->items.at(i).background, lock_tile_datas(0x20, pData->netID, 0, 0, true, 100));
																			if (world->items.at(i).foreground == 4802) {
																				for (ENetPeer* currentPeer2 = server->peers; currentPeer2 < &server->peers[server->peerCount]; ++currentPeer2) {
																					if (currentPeer2->state != ENET_PEER_STATE_CONNECTED) continue;
																					if (isHere(peer, currentPeer2)) {
																						send_rainbow_shit_data(currentPeer2, world->items.at(i).foreground, world->items.at(i).background, i % world->width, i / world->width, false, -1);
																					}
																				}
																				for (ENetPeer* currentPeer2 = server->peers; currentPeer2 < &server->peers[server->peerCount]; ++currentPeer2) {
																					if (currentPeer2->state != ENET_PEER_STATE_CONNECTED) continue;
																					if (isHere(peer, currentPeer2)) {
																						send_rainbow_shit_data(currentPeer2, world->items.at(i).foreground, world->items.at(i).background, i % world->width, i / world->width, true, pData->netID);
																					}
																				}
																			}
																			break;
																		}
																	}
																}
																else {
																	updateplayerset(currentPeer, f.id);
																	auto success = true;
																	SaveItemMoreTimes(f.id, f.count, peer, success);
																}
																for (ENetPeer* currentPeer2 = server->peers; currentPeer2 < &server->peers[server->peerCount]; ++currentPeer2) {
																	if (currentPeer2->state != ENET_PEER_STATE_CONNECTED) continue;
																	if (isHere(peer, currentPeer2)) {
																		Player::PlayAudio(currentPeer2, "audio/change_clothes.wav", duration);
																		SendTradeEffect(currentPeer2, f.id, static_cast<PlayerInfo*>(currentPeer->data)->netID, static_cast<PlayerInfo*>(peer->data)->netID, duration);
																	}
																}
																duration += 500;
															} if (illegal_trade) {
																end_trade(currentPeer);
																break;
															}
															for (auto& f : static_cast<PlayerInfo*>(peer->data)->tradeItems) {
																auto contains = false;
																SearchInventoryItem(peer, f.id, f.count, contains);
																if (!contains) {
																	illegal_trade = true;
																	break;
																}
																RemoveInventoryItem(f.id, f.count, peer, true);
																if (f.id == 1424) { /*worldtrade*/
																	for (ENetPeer* currentPeer2 = server->peers; currentPeer2 < &server->peers[server->peerCount]; ++currentPeer2) {
																		if (currentPeer2->state != ENET_PEER_STATE_CONNECTED) continue;
																		if (isHere(peer, currentPeer2)) {
																			Player::OnTalkBubble(currentPeer2, pData->netID, "`5[```w" + world->name + "`` has been `$World Locked`` by " + static_cast<PlayerInfo*>(currentPeer->data)->displayName + "`5]``", 0, false);
																			Player::OnConsoleMessage(currentPeer2, "`5[```w" + world->name + "`` has been `$World Locked`` by " + static_cast<PlayerInfo*>(currentPeer->data)->displayName + "`5]``");
																			Player::PlayAudio(currentPeer2, "audio/use_lock.wav", 0);
																		}
																	}
																	pData->worldsowned.erase(std::remove(pData->worldsowned.begin(), pData->worldsowned.end(), pData->currentWorld), pData->worldsowned.end());
																	world->owner = static_cast<PlayerInfo*>(currentPeer->data)->rawName;
																	static_cast<PlayerInfo*>(currentPeer->data)->worldsowned.push_back(static_cast<PlayerInfo*>(currentPeer->data)->currentWorld);
																	if (pData->NickPrefix == "") updateworldremove(peer);
																	if (static_cast<PlayerInfo*>(currentPeer->data)->displayName.find("`") != string::npos) {}
																	else {
																		static_cast<PlayerInfo*>(currentPeer->data)->displayName = "`2" + static_cast<PlayerInfo*>(currentPeer->data)->displayName;
																		Player::OnNameChanged(currentPeer, static_cast<PlayerInfo*>(currentPeer->data)->netID, static_cast<PlayerInfo*>(currentPeer->data)->displayName);
																	}
																	for (int i = 0; i < world->width * world->height; i++) {
																		if (getItemDef(world->items.at(i).foreground).blockType == BlockTypes::LOCK) {
																			if (world->items.at(i).foreground == 202 || world->items.at(i).foreground == 204 || world->items.at(i).foreground == 206 || world->items.at(i).foreground == 4994) continue;
																			PlayerMoving moving{};
																			moving.packetType = 0x3;
																			moving.characterState = 0x0;
																			moving.x = i % world->width;
																			moving.y = i / world->width;
																			moving.punchX = i % world->width;
																			moving.punchY = i / world->width;
																			moving.XSpeed = 0;
																			moving.YSpeed = 0;
																			moving.netID = -1;
																			moving.plantingTree = world->items.at(i).foreground;
																			SendPacketRaw(4, packPlayerMoving(&moving), 56, nullptr, peer, ENET_PACKET_FLAG_RELIABLE);
																			send_tile_data(currentPeer, i % world->width, i / world->width, 0x10, world->items.at(i).foreground, world->items.at(i).background, lock_tile_datas(0x20, static_cast<PlayerInfo*>(currentPeer->data)->netID, 0, 0, true, 100));
																			if (world->items.at(i).foreground == 4802) {
																				for (ENetPeer* currentPeer2 = server->peers; currentPeer2 < &server->peers[server->peerCount]; ++currentPeer2) {
																					if (currentPeer2->state != ENET_PEER_STATE_CONNECTED) continue;
																					if (isHere(peer, currentPeer2)) {
																						send_rainbow_shit_data(currentPeer2, world->items.at(i).foreground, world->items.at(i).background, i % world->width, i / world->width, false, -1);
																					}
																				}
																				for (ENetPeer* currentPeer2 = server->peers; currentPeer2 < &server->peers[server->peerCount]; ++currentPeer2) {
																					if (currentPeer2->state != ENET_PEER_STATE_CONNECTED) continue;
																					if (isHere(peer, currentPeer2)) {
																						send_rainbow_shit_data(currentPeer2, world->items.at(i).foreground, world->items.at(i).background, i % world->width, i / world->width, true, static_cast<PlayerInfo*>(currentPeer->data)->netID);
																					}
																				}
																			}
																			break;
																		}
																	}
																}
																else {
																	updateplayerset(peer, f.id);
																	auto success = true;
																	SaveItemMoreTimes(f.id, f.count, currentPeer, success);
																}
																for (ENetPeer* currentPeer2 = server->peers; currentPeer2 < &server->peers[server->peerCount]; ++currentPeer2) {
																	if (currentPeer2->state != ENET_PEER_STATE_CONNECTED) continue;
																	if (isHere(peer, currentPeer2)) {
																		Player::PlayAudio(currentPeer2, "audio/change_clothes.wav", duration);
																		SendTradeEffect(currentPeer2, f.id, static_cast<PlayerInfo*>(peer->data)->netID, static_cast<PlayerInfo*>(currentPeer->data)->netID, duration);
																	}
																}
																duration += 500;
															} if (illegal_trade) {
																end_trade(peer);
																break;
															}
															static_cast<PlayerInfo*>(peer->data)->trade_netid = -1;
															static_cast<PlayerInfo*>(peer->data)->trade = false;
															static_cast<PlayerInfo*>(peer->data)->tradeItems.clear();
															static_cast<PlayerInfo*>(peer->data)->trade_accept = false;
															static_cast<PlayerInfo*>(peer->data)->trade_confirmed = false;
															static_cast<PlayerInfo*>(currentPeer->data)->trade_netid = -1;
															static_cast<PlayerInfo*>(currentPeer->data)->trade = false;
															static_cast<PlayerInfo*>(currentPeer->data)->tradeItems.clear();
															static_cast<PlayerInfo*>(currentPeer->data)->trade_accept = false;
															static_cast<PlayerInfo*>(currentPeer->data)->trade_confirmed = false;
														}
														else if (!static_cast<PlayerInfo*>(currentPeer->data)->trade_confirmed) {
															bool aha = false;
															for (auto& f : static_cast<PlayerInfo*>(currentPeer->data)->tradeItems) {
																if (f.id == 1424) {
																	pData->trade_confirmed = false;
																	Player::OnTextOverlay(peer, "`6[``you must wait for the other player to accept first`6]``");
																	Player::OnTalkBubble(currentPeer, static_cast<PlayerInfo*>(currentPeer->data)->netID, "`6[``You must accept first because you're selling a World Key`6]``", 0, false);
																	Player::OnConsoleMessage(currentPeer, "`6[``You must accept first because you're selling a World Key`6]``");

																	string warning1 = "", warning2 = "", warningkeycurrent = "", warningkeypeer = "", lockedwith = "";
																	if (static_cast<PlayerInfo*>(currentPeer->data)->tradeItems.size() == 0) {
																		warning1 = "\nadd_spacer|small|\nadd_textbox|`4SCAM WARNING: ``You are about to do a trade without receiving anything in return. Once you do the trade you cannot get the items back.|left|\nadd_textbox|`4Do you really want to do this?``|left|\nadd_spacer|small|";
																	}
																	if (pData->tradeItems.size() == 0) {
																		warning2 = "\nadd_spacer|small|\nadd_textbox|`4SCAM WARNING: ``You are about to do a trade without receiving anything in return. Once you do the trade you cannot get the items back.|left|\nadd_textbox|`4Do you really want to do this?``|left|\nadd_spacer|small|";
																	}
																	for (auto& f : static_cast<PlayerInfo*>(peer->data)->tradeItems) {
																		if (f.id == 1424) {
																			for (int i = 0; i < world->width * world->height; i++) {
																				if (getItemDef(world->items.at(i).foreground).blockType == BlockTypes::LOCK) {
																					if (world->items.at(i).foreground == 202 || world->items.at(i).foreground == 204 || world->items.at(i).foreground == 206 || world->items.at(i).foreground == 4994) continue;
																					lockedwith = getItemDef(world->items.at(i).foreground).name;
																					break;
																				}
																			}
																			warningkeycurrent = "\nadd_textbox|`4SCAM WARNING: ``You are buying this world, `#" + world->name + "``. Don't buy a world just to get its rare items, because scammers can easily place secret doors allowing someone to jump in and `4steal the items back`` after a trade!|left|\nadd_spacer|small|\nadd_textbox|Also, all ratings will be removed from a world when it is traded. Don't buy a world for its ranking!|left|\nadd_spacer|small|\nadd_textbox|To be safe, only buy a world for its name and placed blocks, not loose items or high ratings. Consider yourself warned!|left|\nadd_spacer|small|\nadd_textbox|This world is locked by " + lockedwith + ".|left|\nadd_spacer|small|";
																			warningkeypeer = "\nadd_textbox|`4WARNING: ``You are about to sell your world `#" + world->name + "`` - the world lock ownership will be transferred over to `w" + static_cast<PlayerInfo*>(currentPeer->data)->displayName + "`o.|left|\nadd_spacer|small|";
																			break;
																		}
																	}
																	for (auto& f : static_cast<PlayerInfo*>(currentPeer->data)->tradeItems) {
																		if (f.id == 1424) {
																			for (int i = 0; i < world->width * world->height; i++) {
																				if (getItemDef(world->items.at(i).foreground).blockType == BlockTypes::LOCK) {
																					if (world->items.at(i).foreground == 202 || world->items.at(i).foreground == 204 || world->items.at(i).foreground == 206 || world->items.at(i).foreground == 4994) continue;
																					lockedwith = getItemDef(world->items.at(i).foreground).name;
																					break;
																				}
																			}
																			warningkeypeer = "\nadd_textbox|`4SCAM WARNING: ``You are buying this world, `#" + world->name + "``. Don't buy a world just to get its rare items, because scammers can easily place secret doors allowing someone to jump in and `4steal the items back`` after a trade!|left|\nadd_spacer|small|\nadd_textbox|Also, all ratings will be removed from a world when it is traded. Don't buy a world for its ranking!|left|\nadd_spacer|small|\nadd_textbox|To be safe, only buy a world for its name and placed blocks, not loose items or high ratings. Consider yourself warned!|left|\nadd_spacer|small|\nadd_textbox|This world is locked by " + lockedwith + ".|left|\nadd_spacer|small|";
																			warningkeycurrent = "\nadd_textbox|`4WARNING: ``You are about to sell your world `#" + world->name + "`` - the world lock ownership will be transferred over to `w" + pData->displayName + "`o.|left|\nadd_spacer|small|";
																			break;
																		}
																	}

																	Player::OnDialogRequest(peer, "set_default_color|`o\nadd_label_with_icon|big|`wTrade Confirmation``|left|1366|\nadd_spacer|small|\nadd_textbox|`4You'll give:``|left|\nadd_spacer|small|" + FormatTradeItemsForDialog(pData->tradeItems, 0) + "\nadd_spacer|small|\nadd_textbox|`2You'll get:``|left|\nadd_spacer|small|" + FormatTradeItemsForDialog(static_cast<PlayerInfo*>(currentPeer->data)->tradeItems, 1) + "\nadd_spacer|small|" + warningkeypeer + warning1 + "\nadd_button|accept|Do The Trade!|noflags|0|0|\nadd_button|back|Cancel|noflags|0|0|\nend_dialog|trade_confirm|||");

																	aha = true;
																	break;
																}
															} if (!aha) {
																Player::OnTalkBubble(peer, pData->netID, "`6[``Trade accepted, waiting for other player to accept`6]``", 0, false);
																Player::OnConsoleMessage(peer, "`6[``Trade accepted, waiting for other player to accept`6]``");
																Player::OnTalkBubble(currentPeer, static_cast<PlayerInfo*>(currentPeer->data)->netID, "`6[``Trade accepted by other player, waiting for you`6]``", 0, false);
																Player::OnConsoleMessage(currentPeer, "`6[``Trade accepted by other player, waiting for you`6]``");
															}
														}
														break;
													}
												}
											}
											break;
										}
									}
									if (wizardDialog) {
										int x = pData->lastPunchX;
										int y = pData->lastPunchY;
										if (isWorldOwner(peer, world) || isWorldAdmin(peer, world) || admin(peer) || world->owner == "" || !restricted_area(peer, world, x, y) || world->items.at(x + (y * world->width)).foreground == 1790) {
											if (infoDat.at(1) == "honor") {
												pData->choosing_quest = "honor";
												Player::OnDialogRequest(peer, "set_default_color|`o\nadd_label_with_icon|big|`9Quest For Honor``|left|1790|\nadd_textbox|`oThis quest will challenge every fiber of your Growtopian being. It will cost you thousands of gems, weeks or months of time, and possibly your friends and family.<CR>Every quest has 20 steps to complete, and each step alone is probably more than most Growtopians could manage.<CR>But the rewards are also vast. If you complete this quest, you will earn the `9Legendary Title`o!<CR>These quest rewards are `5Untradeable`o, and you will truly be a Legendary Growtopian if you complete a quest.<CR>You may turn in your quests at any Legendary Wizard you have access to (we're in a union), but i will vanish permanently if somebody turns in their final quest step to me, so don't let other people access to me!<CR>There's one last thing you should know before you begin. You can quit your quest at any time, but be aware that if you do, you'll lose all progress on this quest. So choose your quest wisely and see it through to the end!|\nadd_spacer|small|\nadd_label|big|`oSo... now that you've received the official disclaimer, are you truly prepared to embark on the Quest For Honor?``|\nend_dialog|legendary_wizard|No!|Yes!|");
												break;
											}
											else if (infoDat.at(1) == "deliver") {
												if (pData->lastquest == "honor" && pData->quest_step == 1) { /*sand*/
													auto KiekTuri = 0;
													for (auto i = 0; i < pData->inventory.items.size(); i++) {
														if (pData->inventory.items.at(i).itemID == 442 && pData->inventory.items.at(i).itemCount > 0) {
															KiekTuri = pData->inventory.items.at(i).itemCount;
															break;
														}
													}
													if (KiekTuri != 0) {
														if (pData->quest_progress + KiekTuri > 2000) KiekTuri = 2000 - pData->quest_progress;
														RemoveInventoryItem(442, KiekTuri, peer, true);
														pData->quest_progress += KiekTuri;
														if (pData->quest_progress >= 2000) {
															pData->quest_progress = 0;
															pData->quest_step++;
															Player::OnTextOverlay(peer, "`9Quest step complete!!");
															for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
																if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
																if (isHere(peer, currentPeer)) {
																	Player::OnParticleEffect(peer, 48, pData->x, pData->y, 0);
																}
															}
															send_quest_view(peer, pData, world);
														}
														else {
															Player::OnTextOverlay(peer, "`9Thanks! Keep it coming!");
														}
													}
													else {
														goto goodluck;
													}
												}
												else if (pData->lastquest == "honor" && pData->quest_step == 2) { /*pvp*/
													if (pData->quest_progress >= 100) {
														pData->quest_progress = 0;
														pData->quest_step++;
														Player::OnTextOverlay(peer, "`9Quest step complete!!");
														for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
															if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
															if (isHere(peer, currentPeer)) {
																Player::OnParticleEffect(peer, 48, pData->x, pData->y, 0);
															}
														}
														send_quest_view(peer, pData, world);
													}
													else {
														goto goodluck;
													}
												}
												else if (pData->lastquest == "honor" && pData->quest_step == 3) { /*break*/
													if (pData->quest_progress >= 5000) {
														pData->quest_progress = 0;
														pData->quest_step++;
														Player::OnTextOverlay(peer, "`9Quest step complete!!");
														for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
															if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
															if (isHere(peer, currentPeer)) {
																Player::OnParticleEffect(peer, 48, pData->x, pData->y, 0);
															}
														}
														send_quest_view(peer, pData, world);
													}
													else {
														goto goodluck;
													}
												}
												else if (pData->lastquest == "honor" && pData->quest_step == 4) { /*displaybox*/
													auto KiekTuri = 0;
													for (auto i = 0; i < pData->inventory.items.size(); i++) {
														if (pData->inventory.items.at(i).itemID == 1422 && pData->inventory.items.at(i).itemCount > 0) {
															KiekTuri = pData->inventory.items.at(i).itemCount;
															break;
														}
													}
													if (KiekTuri != 0) {
														if (pData->quest_progress + KiekTuri > 600) KiekTuri = 600 - pData->quest_progress;
														RemoveInventoryItem(1422, KiekTuri, peer, true);
														pData->quest_progress += KiekTuri;
														if (pData->quest_progress >= 600) {
															pData->quest_progress = 0;
															pData->quest_step++;
															Player::OnTextOverlay(peer, "`9Quest step complete!!");
															for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
																if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
																if (isHere(peer, currentPeer)) {
																	Player::OnParticleEffect(peer, 48, pData->x, pData->y, 0);
																}
															}
															send_quest_view(peer, pData, world);
														}
														else {
															Player::OnTextOverlay(peer, "`9Thanks! Keep it coming!");
														}
													}
													else {
														goto goodluck;
													}
												}
												else if (pData->lastquest == "honor" && pData->quest_step == 5) { /*plant*/
													if (pData->quest_progress >= 50000) {
														pData->quest_progress = 0;
														pData->quest_step++;
														Player::OnTextOverlay(peer, "`9Quest step complete!!");
														for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
															if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
															if (isHere(peer, currentPeer)) {
																Player::OnParticleEffect(peer, 48, pData->x, pData->y, 0);
															}
														}
														send_quest_view(peer, pData, world);
													}
													else {
														goto goodluck;
													}
												}
												else if (pData->lastquest == "honor" && pData->quest_step == 6) { /*growtoken*/
													if (pData->quest_progress >= 50) {
														pData->quest_progress = 0;
														pData->quest_step++;
														Player::OnTextOverlay(peer, "`9Quest step complete!!");
														for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
															if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
															if (isHere(peer, currentPeer)) {
																Player::OnParticleEffect(peer, 48, pData->x, pData->y, 0);
															}
														}
														send_quest_view(peer, pData, world);
													}
													else {
														goto goodluck;
													}
												}
												else if (pData->lastquest == "honor" && pData->quest_step == 7) { /*Goldendiaper*/
													auto KiekTuri = 0;
													for (auto i = 0; i < pData->inventory.items.size(); i++) {
														if (pData->inventory.items.at(i).itemID == 1462 && pData->inventory.items.at(i).itemCount > 0) {
															KiekTuri = pData->inventory.items.at(i).itemCount;
															break;
														}
													}
													if (KiekTuri != 0) {
														if (pData->quest_progress + KiekTuri > 3) KiekTuri = 3 - pData->quest_progress;
														RemoveInventoryItem(1462, KiekTuri, peer, true);
														pData->quest_progress += KiekTuri;
														if (pData->quest_progress >= 3) {
															pData->quest_progress = 0;
															pData->quest_step++;
															Player::OnTextOverlay(peer, "`9Quest step complete!!");
															for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
																if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
																if (isHere(peer, currentPeer)) {
																	Player::OnParticleEffect(peer, 48, pData->x, pData->y, 0);
																}
															}
															send_quest_view(peer, pData, world);
														}
														else {
															Player::OnTextOverlay(peer, "`9Thanks! Keep it coming!");
														}
													}
													else {
														goto goodluck;
													}
												}
												else if (pData->lastquest == "honor" && pData->quest_step == 8) { /*xp*/
													if (pData->quest_progress >= 10000) {
														pData->quest_progress = 0;
														pData->quest_step++;
														Player::OnTextOverlay(peer, "`9Quest step complete!!");
														for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
															if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
															if (isHere(peer, currentPeer)) {
																Player::OnParticleEffect(peer, 48, pData->x, pData->y, 0);
															}
														}
														send_quest_view(peer, pData, world);
													}
													else {
														goto goodluck;
													}
												}
												else if (pData->lastquest == "honor" && pData->quest_step == 9) { /*Tombstone*/
													auto KiekTuri = 0;
													for (auto i = 0; i < pData->inventory.items.size(); i++) {
														if (pData->inventory.items.at(i).itemID == 784 && pData->inventory.items.at(i).itemCount > 0) {
															KiekTuri = pData->inventory.items.at(i).itemCount;
															break;
														}
													}
													if (KiekTuri != 0) {
														if (pData->quest_progress + KiekTuri > 800) KiekTuri = 800 - pData->quest_progress;
														RemoveInventoryItem(784, KiekTuri, peer, true);
														pData->quest_progress += KiekTuri;
														if (pData->quest_progress >= 800) {
															pData->quest_progress = 0;
															pData->quest_step++;
															Player::OnTextOverlay(peer, "`9Quest step complete!!");
															for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
																if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
																if (isHere(peer, currentPeer)) {
																	Player::OnParticleEffect(peer, 48, pData->x, pData->y, 0);
																}
															}
															send_quest_view(peer, pData, world);
														}
														else {
															Player::OnTextOverlay(peer, "`9Thanks! Keep it coming!");
														}
													}
													else {
														goto goodluck;
													}
												}
												else if (pData->lastquest == "honor" && pData->quest_step == 10) { /*Gems*/
													auto KiekTuri = 0;
													auto KiekTuri2 = 0;
													std::ifstream ifsz("save/gemdb/_" + pData->rawName + ".txt");
													std::string content((std::istreambuf_iterator<char>(ifsz)), (std::istreambuf_iterator<char>()));
													KiekTuri = atoi(content.c_str());
													KiekTuri2 = atoi(content.c_str());
													if (KiekTuri != 0) {
														if (pData->quest_progress + KiekTuri > 100000) KiekTuri = 100000 - pData->quest_progress;
														auto fingembux = KiekTuri2 - KiekTuri;
														ofstream myfile;
														myfile.open("save/gemdb/_" + pData->rawName + ".txt");
														myfile << fingembux;
														myfile.close();
														int gemcalc = KiekTuri2 - KiekTuri;
														Player::OnSetBux(peer, gemcalc, 0);
														pData->quest_progress += KiekTuri;
														if (pData->quest_progress >= 100000) {
															pData->quest_progress = 0;
															pData->quest_step++;
															Player::OnTextOverlay(peer, "`9Quest step complete!!");
															for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
																if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
																if (isHere(peer, currentPeer)) {
																	Player::OnParticleEffect(peer, 48, pData->x, pData->y, 0);
																}
															}
															send_quest_view(peer, pData, world);
														}
														else {
															Player::OnTextOverlay(peer, "`9Thanks! Keep it coming!");
														}
													}
													else {
														goto goodluck;
													}
												}
												else if (pData->lastquest == "honor" && pData->quest_step == 11) { /*break*/
													if (pData->quest_progress >= 100000) {
														pData->quest_progress = 0;
														pData->quest_step++;
														Player::OnTextOverlay(peer, "`9Quest step complete!!");
														for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
															if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
															if (isHere(peer, currentPeer)) {
																Player::OnParticleEffect(peer, 48, pData->x, pData->y, 0);
															}
														}
														send_quest_view(peer, pData, world);
													}
													else {
														goto goodluck;
													}
												}
												else if (pData->lastquest == "honor" && pData->quest_step == 12) { /*surgery*/
													if (pData->quest_progress >= 100) {
														pData->quest_progress = 0;
														pData->quest_step++;
														Player::OnTextOverlay(peer, "`9Quest step complete!!");
														for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
															if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
															if (isHere(peer, currentPeer)) {
																Player::OnParticleEffect(peer, 48, pData->x, pData->y, 0);
															}
														}
														send_quest_view(peer, pData, world);
													}
													else {
														goto goodluck;
													}
												}
												else if (pData->lastquest == "honor" && pData->quest_step == 13) { /*provider*/
													if (pData->quest_progress >= 1000) {
														pData->quest_progress = 0;
														pData->quest_step++;
														Player::OnTextOverlay(peer, "`9Quest step complete!!");
														for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
															if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
															if (isHere(peer, currentPeer)) {
																Player::OnParticleEffect(peer, 48, pData->x, pData->y, 0);
															}
														}
														send_quest_view(peer, pData, world);
													}
													else {
														goto goodluck;
													}
												}
												else if (pData->lastquest == "honor" && pData->quest_step == 14) { /*Golden Heart Crystal*/
													auto KiekTuri = 0;
													for (auto i = 0; i < pData->inventory.items.size(); i++) {
														if (pData->inventory.items.at(i).itemID == 1458 && pData->inventory.items.at(i).itemCount > 0) {
															KiekTuri = pData->inventory.items.at(i).itemCount;
															break;
														}
													}
													if (KiekTuri != 0) {
														if (pData->quest_progress + KiekTuri > 3) KiekTuri = 3 - pData->quest_progress;
														RemoveInventoryItem(1458, KiekTuri, peer, true);
														pData->quest_progress += KiekTuri;
														if (pData->quest_progress >= 3) {
															pData->quest_progress = 0;
															pData->quest_step++;
															Player::OnTextOverlay(peer, "`9Quest step complete!!");
															for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
																if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
																if (isHere(peer, currentPeer)) {
																	Player::OnParticleEffect(peer, 48, pData->x, pData->y, 0);
																}
															}
															send_quest_view(peer, pData, world);
														}
														else {
															Player::OnTextOverlay(peer, "`9Thanks! Keep it coming!");
														}
													}
													else {
														goto goodluck;
													}
												}
												else if (pData->lastquest == "honor" && pData->quest_step == 15) { /*fruit*/
													if (pData->quest_progress >= 100000) {
														pData->quest_progress = 0;
														pData->quest_step++;
														Player::OnTextOverlay(peer, "`9Quest step complete!!");
														for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
															if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
															if (isHere(peer, currentPeer)) {
																Player::OnParticleEffect(peer, 48, pData->x, pData->y, 0);
															}
														}
														send_quest_view(peer, pData, world);
													}
													else {
														goto goodluck;
													}
												}
												else if (pData->lastquest == "honor" && pData->quest_step == 16) { /*Growieaward*/
													auto KiekTuri = 0;
													for (auto i = 0; i < pData->inventory.items.size(); i++) {
														if (pData->inventory.items.at(i).itemID == 1614 && pData->inventory.items.at(i).itemCount > 0) {
															KiekTuri = pData->inventory.items.at(i).itemCount;
															break;
														}
													}
													if (KiekTuri != 0) {
														if (pData->quest_progress + KiekTuri > 1) KiekTuri = 1 - pData->quest_progress;
														RemoveInventoryItem(1614, KiekTuri, peer, true);
														pData->quest_progress += KiekTuri;
														if (pData->quest_progress >= 1) {
															pData->quest_progress = 0;
															pData->quest_step++;
															Player::OnTextOverlay(peer, "`9Quest step complete!!");
															for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
																if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
																if (isHere(peer, currentPeer)) {
																	Player::OnParticleEffect(peer, 48, pData->x, pData->y, 0);
																}
															}
															send_quest_view(peer, pData, world);
														}
														else {
															Player::OnTextOverlay(peer, "`9Thanks! Keep it coming!");
														}
													}
													else {
														goto goodluck;
													}
												}
												else if (pData->lastquest == "honor" && pData->quest_step == 17) { /*SuperFirework*/
													auto KiekTuri = 0;
													for (auto i = 0; i < pData->inventory.items.size(); i++) {
														if (pData->inventory.items.at(i).itemID == 1680 && pData->inventory.items.at(i).itemCount > 0) {
															KiekTuri = pData->inventory.items.at(i).itemCount;
															break;
														}
													}
													if (KiekTuri != 0) {
														if (pData->quest_progress + KiekTuri > 3) KiekTuri = 3 - pData->quest_progress;
														RemoveInventoryItem(1680, KiekTuri, peer, true);
														pData->quest_progress += KiekTuri;
														if (pData->quest_progress >= 3) {
															pData->quest_progress = 0;
															pData->quest_step++;
															Player::OnTextOverlay(peer, "`9Quest step complete!!");
															for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
																if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
																if (isHere(peer, currentPeer)) {
																	Player::OnParticleEffect(peer, 48, pData->x, pData->y, 0);
																}
															}
															send_quest_view(peer, pData, world);
														}
														else {
															Player::OnTextOverlay(peer, "`9Thanks! Keep it coming!");
														}
													}
													else {
														goto goodluck;
													}
												}
												else if (pData->lastquest == "honor" && pData->quest_step == 18) { /*RainbowWing*/
													auto KiekTuri = 0;
													for (auto i = 0; i < pData->inventory.items.size(); i++) {
														if (pData->inventory.items.at(i).itemID == 1672 && pData->inventory.items.at(i).itemCount > 0) {
															KiekTuri = pData->inventory.items.at(i).itemCount;
															break;
														}
													}
													if (KiekTuri != 0) {
														if (pData->quest_progress + KiekTuri > 3) KiekTuri = 3 - pData->quest_progress;
														RemoveInventoryItem(1672, KiekTuri, peer, true);
														pData->quest_progress += KiekTuri;
														if (pData->quest_progress >= 3) {
															pData->quest_progress = 0;
															pData->quest_step++;
															Player::OnTextOverlay(peer, "`9Quest step complete!!");
															for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
																if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
																if (isHere(peer, currentPeer)) {
																	Player::OnParticleEffect(peer, 48, pData->x, pData->y, 0);
																}
															}
															send_quest_view(peer, pData, world);
														}
														else {
															Player::OnTextOverlay(peer, "`9Thanks! Keep it coming!");
														}
													}
													else {
														goto goodluck;
													}
												}
												else if (pData->lastquest == "honor" && pData->quest_step == 19) { /*BirthCertificate*/
													auto KiekTuri = 0;
													for (auto i = 0; i < pData->inventory.items.size(); i++) {
														if (pData->inventory.items.at(i).itemID == 1280 && pData->inventory.items.at(i).itemCount > 0) {
															KiekTuri = pData->inventory.items.at(i).itemCount;
															break;
														}
													}
													if (KiekTuri != 0) {
														if (pData->quest_progress + KiekTuri > 3) KiekTuri = 3 - pData->quest_progress;
														RemoveInventoryItem(1280, KiekTuri, peer, true);
														pData->quest_progress += KiekTuri;
														if (pData->quest_progress >= 3) {
															pData->quest_progress = 0;
															pData->quest_step++;
															Player::OnTextOverlay(peer, "`9Quest step complete!!");
															for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
																if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
																if (isHere(peer, currentPeer)) {
																	Player::OnParticleEffect(peer, 48, pData->x, pData->y, 0);
																}
															}
															send_quest_view(peer, pData, world);
														}
														else {
															Player::OnTextOverlay(peer, "`9Thanks! Keep it coming!");
														}
													}
													else {
														goto goodluck;
													}
												}
												else if (pData->lastquest == "honor" && pData->quest_step == 20) { /*LegendaryOrb*/
													auto KiekTuri = 0;
													for (auto i = 0; i < pData->inventory.items.size(); i++) {
														if (pData->inventory.items.at(i).itemID == 1794 && pData->inventory.items.at(i).itemCount > 0) {
															KiekTuri = pData->inventory.items.at(i).itemCount;
															break;
														}
													}
													if (KiekTuri != 0) {
														if (pData->quest_progress + KiekTuri > 1) KiekTuri = 1 - pData->quest_progress;
														RemoveInventoryItem(1794, KiekTuri, peer, true);
														pData->quest_progress += KiekTuri;
														if (pData->quest_progress >= 1) {
															pData->quest_progress = 0;
															pData->quest_step = 0;
															pData->quest_active = false;
															pData->lastquest = "";
															Player::OnTextOverlay(peer, "`9Quest step complete!!");
															world->items.at(x + (y * world->width)).foreground = 0;
															PlayerMoving data3{};
															data3.packetType = 0x3;
															data3.characterState = 0x0;
															data3.x = x;
															data3.y = y;
															data3.punchX = x;
															data3.punchY = y;
															data3.XSpeed = 0;
															data3.YSpeed = 0;
															data3.netID = -1;
															data3.plantingTree = 0;
															pData->ltitle = true;
															pData->ltitleunlocked = true;
															string previous_name = pData->displayName;
															if (pData->ltitle && pData->ltitleunlocked && pData->displayName.find(" of Legend``") == string::npos) {
																pData->displayName += " of Legend``";
															} if (isWorldOwner(peer, world)) {
																pData->displayName = "`2" + pData->displayName;
																Player::OnNameChanged(peer, pData->netID, "`2" + pData->displayName);
															}
															else {
																Player::OnNameChanged(peer, pData->netID, pData->displayName);
															}
															for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
																if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
																if (isHere(peer, currentPeer)) {
																	auto raw = packPlayerMoving(&data3);
																	raw[2] = 0;
																	raw[3] = 0;
																	Player::OnParticleEffect(currentPeer, 48, pData->x, pData->y, 0);
																	Player::OnParticleEffect(currentPeer, 90, pData->x, pData->y, 0);
																	Player::OnTalkBubble(currentPeer, pData->netID, "`2" + previous_name + " `5earned the achievement 'DARY! (Classic)'!", 0, true);
																	SendPacketRaw(4, raw, 56, nullptr, currentPeer, ENET_PACKET_FLAG_RELIABLE);
																}
															}
														}
														else {
															Player::OnTextOverlay(peer, "`9Thanks! Keep it coming!");
														}
													}
													else {
														goto goodluck;
													}
												}
												else {
												goodluck:
													Player::OnTextOverlay(peer, "`9Good luck! You can do it!");
												}
												break;
											}
											else if (infoDat.at(1) == "give_up") {
												break;
											}
											else if (pData->choosing_quest == "honor") {
												pData->quest_active = true;
												pData->lastquest = "honor";
												pData->quest_step = 1;
												pData->quest_progress = 0;
												send_quest_view(peer, pData, world);
												break;
											}
										}
									}
									if (isTitleDialog) {
										if (infoDat.at(1) == "edit_title") {
											if (!pData->Subscriber) break;
											Player::OnDialogRequest(peer, "set_default_color|`o\n\nadd_label|big|`oEdit Your Title``|\nadd_textbox|`oHow to create a `$Title`o? Firstly make sure what `$Color `oYou want to use for example `2Green `oOr `4Red `oThen in the `$Input Box `oWrite your new `$Title `oLets say we want to use `4King `oAnd then simply press `$Save`o!|\nadd_spacer|small|\nadd_text_input|title_name|`$Title``||8|\nend_dialog|title_save|Cancel|Save|");
											break;
										}
										if (infoDat.at(0) == "legendary_title") {
											if (pData->ltitleunlocked) {
												if (pData->ltitle && infoDat.at(1) == "0") {
													pData->ltitle = false;
													size_t pos;
													while ((pos = pData->displayName.find(" of Legend``")) != string::npos) {
														pData->displayName.replace(pos, 12, "");
													} if (isWorldOwner(peer, world)) {
														pData->displayName = "`2" + pData->displayName;
														Player::OnNameChanged(peer, pData->netID, "`2" + pData->displayName);
													}
													else {
														Player::OnNameChanged(peer, pData->netID, pData->displayName);
													}
													continue;
												}
												else if (!pData->ltitle && infoDat.at(1) == "1") {
													pData->ltitle = true;
													if (pData->ltitle && pData->ltitleunlocked && pData->displayName.find(" of Legend``") == string::npos) {
														pData->displayName += " of Legend``";
													} if (isWorldOwner(peer, world)) {
														pData->displayName = "`2" + pData->displayName;
														Player::OnNameChanged(peer, pData->netID, "`2" + pData->displayName);
													}
													else {
														Player::OnNameChanged(peer, pData->netID, pData->displayName);
													}
													continue;
												}
											}
										}
										if (infoDat.at(0) == "blue_name") {
											if (pData->level >= 125) {
												if (pData->bluenametitle && infoDat.at(1) == "0") {
													pData->bluenametitle = false;
													for (auto currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
														if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
														if (isHere(peer, currentPeer)) {
															auto p2ww = packetEnd(appendString(appendString(createPacket(), "OnCountryState"), static_cast<PlayerInfo*>(peer->data)->country + "|showGuild"));
															memcpy(p2ww.data + 8, &(static_cast<PlayerInfo*>(peer->data)->netID), 4);
															const auto packet2ww = enet_packet_create(p2ww.data, p2ww.len, ENET_PACKET_FLAG_RELIABLE);
															enet_peer_send(currentPeer, 0, packet2ww);
															delete p2ww.data;
															auto p2wwee = packetEnd(appendString(appendString(createPacket(), "OnCountryState"), static_cast<PlayerInfo*>(currentPeer->data)->country + "|showGuild"));
															memcpy(p2wwee.data + 8, &(static_cast<PlayerInfo*>(peer->data)->netID), 4);
															const auto packet2wwee = enet_packet_create(p2wwee.data, p2wwee.len, ENET_PACKET_FLAG_RELIABLE);
															enet_peer_send(peer, 0, packet2wwee);
															delete p2wwee.data;
														}
													}
													continue;
												}
												else if (!pData->bluenametitle && infoDat.at(1) == "1") {
													pData->bluenametitle = true;
													for (auto currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
														if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
														if (isHere(peer, currentPeer)) {
															auto p2ww = packetEnd(appendString(appendString(createPacket(), "OnCountryState"), static_cast<PlayerInfo*>(peer->data)->country + "|showGuild|maxLevel"));
															memcpy(p2ww.data + 8, &(static_cast<PlayerInfo*>(peer->data)->netID), 4);
															const auto packet2ww = enet_packet_create(p2ww.data, p2ww.len, ENET_PACKET_FLAG_RELIABLE);
															enet_peer_send(currentPeer, 0, packet2ww);
															delete p2ww.data;
															auto p2wwee = packetEnd(appendString(appendString(createPacket(), "OnCountryState"), static_cast<PlayerInfo*>(currentPeer->data)->country + "|showGuild|maxLevel"));
															memcpy(p2wwee.data + 8, &(static_cast<PlayerInfo*>(peer->data)->netID), 4);
															const auto packet2wwee = enet_packet_create(p2wwee.data, p2wwee.len, ENET_PACKET_FLAG_RELIABLE);
															enet_peer_send(peer, 0, packet2wwee);
															delete p2wwee.data;
														}
													}
													continue;
												}
											}
										}
									}
									if (isRiftDialog) {
										if (infoDat.at(0) == "checkbox_aura0") {
											if (infoDat.at(1) == "0" && pData->rift_type != 0) {
												pData->rift_type = 0;
												for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
													if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
													if (isHere(peer, currentPeer)) {
														send_rift_cape(currentPeer, pData);
													}
												}
												break;
											}
										}
										if (infoDat.at(1) == "restore_default") { /*restore*/
											pData->rift_cape_r = 1;
											pData->rift_cape_g = 1;
											pData->rift_cape_b = 1;
											pData->rift_collar_r = 1;
											pData->rift_collar_g = 1;
											pData->rift_collar_b = 1;
											pData->rift_type = 0;
											pData->rift_cape_color = 111255;
											pData->rift_collar_color = 111255;
											for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
												if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
												if (isHere(peer, currentPeer)) {
													send_rift_cape(currentPeer, pData);
												}
											}
											break;
										}
										if (infoDat.at(0) == "text_input_cape_color0") { /*decimal cape color*/
											auto ex = explode(",", infoDat.at(1));
											string c_code = ex.at(0) + ex.at(1) + ex.at(2) + "255";
											pData->rift_cape_r = atoi(ex.at(0).c_str());
											pData->rift_cape_g = atoi(ex.at(1).c_str());
											pData->rift_cape_b = atoi(ex.at(2).c_str());
											pData->rift_cape_color = atoi(c_code.c_str());
											for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
												if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
												if (isHere(peer, currentPeer)) {
													send_rift_cape(currentPeer, pData);
												}
											}
										}
										if (infoDat.at(0) == "checkbox_aura_1st0") { /*portal*/
											if (infoDat.at(1) == "1" && pData->rift_type != 2555) {
												pData->rift_type = 2555;
												for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
													if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
													if (isHere(peer, currentPeer)) {
														send_rift_cape(currentPeer, pData);
													}
												}
												break;
											}
										}
										if (infoDat.at(0) == "checkbox_aura_2nd0") { /*starfield*/
											if (infoDat.at(1) == "1" && pData->rift_type != 2811) {
												pData->rift_type = 2811;
												for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
													if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
													if (isHere(peer, currentPeer)) {
														send_rift_cape(currentPeer, pData);
													}
												}
												break;
											}
										}
										if (infoDat.at(0) == "checkbox_aura_3rd0") { /*electrical*/
											if (infoDat.at(1) == "1" && pData->rift_type != 3067) {
												pData->rift_type = 3067;
												for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
													if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
													if (isHere(peer, currentPeer)) {
														send_rift_cape(currentPeer, pData);
													}
												}
												break;
											}
										}
										if (infoDat.at(0) == "text_input_collar_color0") { /*decimal collar color*/
											auto ex = explode(",", infoDat.at(1));
											string c_code = ex.at(0) + ex.at(1) + ex.at(2) + "255";
											pData->rift_collar_r = atoi(ex.at(0).c_str());
											pData->rift_collar_g = atoi(ex.at(1).c_str());
											pData->rift_collar_b = atoi(ex.at(2).c_str());
											pData->rift_collar_color = atoi(c_code.c_str());
											for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
												if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
												if (isHere(peer, currentPeer)) {
													send_rift_cape(currentPeer, pData);
												}
											}
										}
									}
									if (createGuildDialog) {
										if (infoDat.at(1) == "createguild") {
											confirm_create = true;
											continue;
										} if (infoDat.at(0) == "guildname") {
											guild_name_c = infoDat.at(1);
											continue;
										} if (infoDat.at(0) == "guilddesc" && confirm_create) {
											string old_name = guild_name_c;
											toUpperCase(guild_name_c);
											if (pData->guild != "") break;
											if (guild_name_c.size() <= 3 || guild_name_c.size() >= 16) {
												Player::OnTalkBubble(peer, pData->netID, "Guild name is too long or too short!", 0, true);
												break;
											} if (infoDat.at(1).size() <= 3 || infoDat.at(1).size() >= 100) {
												Player::OnTalkBubble(peer, pData->netID, "Guild statement is too long or too short!", 0, true);
												break;
											} if (guild_name_c.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789") != string::npos) {
												Player::OnTalkBubble(peer, pData->netID, "You cannot use symbols in your guild name!", 0, true);
												break;
											} if (!isWorldOwner(peer, world)) {
												Player::OnTalkBubble(peer, pData->netID, "You need to be world owner to create a guild!", 0, true);
												break;
											} if (std::experimental::filesystem::exists("save/guilds/_" + guild_name_c + ".json")) {
												Player::OnTalkBubble(peer, pData->netID, "Guild with this name already exists!", 0, true);
												break;
											}
											std::ifstream ifsz("save/gemdb/_" + pData->rawName + ".txt");
											std::string content((std::istreambuf_iterator<char>(ifsz)), (std::istreambuf_iterator<char>()));
											auto gembux = atoi(content.c_str());
											if (gembux < 100000) {
												break;
											}
											auto fingembux = gembux - 100000;
											ofstream myfile;
											myfile.open("save/gemdb/_" + pData->rawName + ".txt");
											myfile << fingembux;
											myfile.close();
											int gemcalc = gembux - 100000;
											Player::OnSetBux(peer, gemcalc, 0);
											try {
												std::ifstream check_guild("save/guilds/_" + guild_name_c + ".json");
												if (check_guild.is_open()) { /*shouldnt be possible but for any incident*/
													Player::OnTalkBubble(peer, pData->netID, "Guild with this name already exists!", 0, true);
													break;
												}
												check_guild.close();
												std::ofstream write_guild("save/guilds/_" + guild_name_c + ".json");
												if (!write_guild.is_open()) {
													Player::OnTalkBubble(peer, pData->netID, "Failed to create a guild, please try again!", 0, true);
													cout << GetLastError() << endl;
													break;
												}
												json j;
												j["guild_name"] = old_name;
												j["guild_world"] = pData->currentWorld;
												j["statement"] = infoDat.at(1);
												j["leader"] = pData->rawName;
												j["foreground"] = 0;
												j["background"] = 0;
												j["members"] = "";
												write_guild << j << std::endl;
												write_guild.close();
											}
											catch (std::exception& e) {
												std::cout << e.what() << std::endl;
												break;
											}
											pData->guild = old_name;
											pData->guildFg = 0;
											pData->guildBg = 0;
											world->category = "Guild";
											Player::OnAddNotification(peer, "`w" + old_name + " guild have been created!", "audio/gong.wav", "interface/large/gd_prom.rttex");
											for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
												if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
												if (isHere(peer, currentPeer)) {
													gamepacket_t p(0, pData->netID);
													p.Insert("OnGuildDataChanged");
													p.Insert(36095);
													p.Insert(63158650);
													p.Insert(0);
													p.Insert(3);
													p.CreatePacket(currentPeer);
												}
											}
											try {
												ifstream read_player("save/players/_" + pData->rawName + ".json");
												if (!read_player.is_open()) {
													continue;
												}
												json j;
												read_player >> j;
												read_player.close();
												j["guild"] = pData->guild;
												ofstream write_player("save/players/_" + pData->rawName + ".json");
												write_player << j << std::endl;
												write_player.close();
											}
											catch (std::exception& e) {
												std::cout << e.what() << std::endl;
												break;
											}
											break;
										}
									}
									if (changepasswordDialog) {
										if (infoDat.at(0) == "current_password") {
											current_password = infoDat.at(1);
										}
										if (infoDat.at(0) == "new_password") {
											new_password = infoDat.at(1);
										}
										if (infoDat.at(0) == "confirm_new_password") {
											new_password_confirm = infoDat.at(1);
											string current_user_password = pData->tankIDPass;
											string normal_one = new_password;
											toUpperCase(new_password_confirm);
											toUpperCase(current_password);
											toUpperCase(current_user_password);
											toUpperCase(new_password);
											if (current_password != current_user_password) {
												Player::OnTalkBubble(peer, pData->netID, "Current password didn't match the real one!", 0, true);
												break;
											}
											if (current_user_password == new_password) {
												Player::OnTalkBubble(peer, pData->netID, "It makes no sense to change existing password to the same one!", 0, true);
												break;
											}
											if (new_password != new_password_confirm) {
												Player::OnTalkBubble(peer, pData->netID, "New password didn't match the confirmation password!", 0, true);
												break;
											}
											if (new_password.length() < 3 || new_password.length() > 20) {
												Player::OnTalkBubble(peer, pData->netID, "New password is too long or too short!", 0, true);
												break;
											}
											Player::OnTalkBubble(peer, pData->netID, "Password changed to " + normal_one + " write it down so you wont forget!", 0, true);
											pData->tankIDPass = new_password;
											GamePacket p8 = packetEnd(appendString(appendString(appendInt(appendString(createPacket(), "SetHasGrowID"), 1), pData->tankIDName), pData->tankIDPass));
											ENetPacket* packet8 = enet_packet_create(p8.data, p8.len, ENET_PACKET_FLAG_RELIABLE);
											enet_peer_send(peer, 0, packet8);
											delete p8.data;
											sendSound(peer, "audio/dialog_confirm.wav");
											break;
										}
									}
									if (isMarketDialog) {
										if (infoDat.at(1).substr(0, 19) == "sellitembyrarityid_") {
											if (!triggersell) {
												triggersell = true;
												continue;
											}
										}
										if (infoDat.at(0).substr(0, 18) == "buyitembyrarityid_") {
											string strButItemByRarityCount = infoDat.at(1);
											string id = infoDat.at(0).substr(infoDat.at(0).find("_") + 1);
											if (triggersell) {
												bool contains_non_int2 = !std::regex_match(strButItemByRarityCount, std::regex("^[0-9]+$"));
												bool contains_non_int4 = !std::regex_match(id, std::regex("^[0-9]+$"));
												if (strButItemByRarityCount == "" || contains_non_int4 || id.size() > 5 || id.size() <= 0 || strButItemByRarityCount.length() > 5 || contains_non_int2 || strButItemByRarityCount.size() > 3 || strButItemByRarityCount.size() <= 0) {
													Player::OnTalkBubble(peer, pData->netID, "You can't sell more than 250 or less than 0!", 0, true);
													break;
												}
												int item_count = atoi(strButItemByRarityCount.c_str());
												int item_id = atoi(id.c_str());
												int itemid = item_id;
												if (getItemDef(itemid).properties & Property_Untradable && getItemDef(itemid).rarity == 999 || getItemDef(itemid).blockType != BlockTypes::CLOTHING && getItemDef(itemid).rarity == 999 && itemid != 5142 && itemid != 7188 && itemid != 9498 && itemid != 5154 && itemid != 5140 && itemid != 5138 && itemid != 5136 && itemid != 7382 && itemid != 4762 || getItemDef(itemid).blockType == BlockTypes::LOCK && itemid != 7188 || itemid == 10034 || getItemDef(itemid).name.find("null") != string::npos || itemid == 10036 || getItemDef(itemid).name.find("Mooncake") != string::npos || getItemDef(itemid).name.find("Harvest") != string::npos && itemid != 1830 || getItemDef(itemid).name.find("Autumn") != string::npos || itemid == 1056 || itemid == 1804 || getItemDef(itemid).blockType == BlockTypes::COMPONENT || getItemDef(itemid).properties & Property_Chemical || itemid == 6920 || itemid == 6922 || itemid == 1874 || itemid == 1876 || itemid == 1904 || itemid == 1932 || itemid == 1900 || itemid == 1986 || itemid == 1996 || itemid == 2970 || itemid == 3140 || itemid == 3174 || itemid == 6028 || itemid == 6846 || itemid == 8962 || itemid == 2408 || itemid == 4428 || itemid == 5086 || itemid == 9240 || itemid == 9306 || itemid == 9290 || itemid == 7328 || itemid == 9416 || itemid == 10386 || itemid == 9410 || itemid == 1458 || itemid == 9408 || itemid == 9360 || itemid == 6866 || itemid == 6868 || itemid == 6870 || itemid == 6872 || itemid == 6874 || itemid == 6876 || itemid == 6878 || itemid == 2480 || itemid == 8452 || itemid == 5132 || itemid == 7166 || itemid == 5126 || itemid == 5128 || itemid == 5130 || itemid == 5144 || itemid == 5146 || itemid == 5148 || itemid == 5150 || itemid == 5162 || itemid == 5164 || itemid == 5166 || itemid == 5168 || itemid == 5180 || itemid == 5182 || itemid == 5184 || itemid == 5186 || itemid == 7168 || itemid == 7170 || itemid == 7172 || itemid == 7174 || itemid == 8834 || itemid == 7912 || itemid == 9212 || itemid == 5134 || itemid == 5152 || itemid == 5170 || itemid == 5188 || itemid == 980 || itemid == 9448 || itemid == 9310 || itemid == 10034 || itemid == 10036 || itemid == 8470 || itemid == 8286 || itemid == 6026 || itemid == 1784 || itemid == 9356 || itemid == 10022 || itemid == 902 || itemid == 10032 || itemid == 834 || itemid == 6 || itemid == 5640 || itemid == 9492 || itemid == 1782 || itemid == 9288 || itemid == 1780 || itemid == 8306 || itemid == 202 || itemid == 204 || itemid == 206 || itemid == 2950 || itemid == 4802 || itemid == 4994 || itemid == 5260 || itemid == 5814 || itemid == 5980 || itemid == 7734 || itemid == 2592 || itemid == 2242 || itemid == 1794 || itemid == 1792 || itemid == 778 || itemid == 9510 || itemid == 8774 || itemid == 2568 || itemid == 9512 || itemid == 9502 || itemid == 9482 || itemid == 2250 || itemid == 2248 || itemid == 2244 || itemid == 2246 || itemid == 2286 || itemid == 9508 || itemid == 9504 || itemid == 9506 || itemid == 274 || itemid == 276 || itemid == 9476 || itemid == 1486 || itemid == 4426 || itemid == 9496 || itemid == 278 || itemid == 9490 || itemid == 2410 || itemid == 9488 || itemid == 9452 || itemid == 9454 || itemid == 9472 || itemid == 9456 || itemid == 732 || itemid == 9458 || itemid == 6336 || itemid == 112 || itemid == 8 || itemid == 3760 || getItemDef(itemid).blockType == BlockTypes::FISH || itemid == 7372 || itemid == 9438 || itemid == 9462 || itemid == 9440 || itemid == 9442 || itemid == 9444 || itemid == 7960 || itemid == 7628 || itemid == 8552) break;
												if (itemid == 6312 || itemid == 998 || itemid == 986 || itemid == 992 || itemid == 990 || itemid == 996 || itemid == 988 || itemid == 1004 || itemid == 1006 || itemid == 1002 || itemid == 9504 || itemid == 9506 || itemid == 9508 || itemid == 9510 || itemid == 9512 || itemid == 9514 || itemid == 9518 || itemid == 9520 || itemid == 9502 || itemid == 9496 || itemid == 1790 || itemid == 9492 || itemid == 9488 || itemid == 9222 || itemid == 1360 || itemid == 6260 || itemid == 822 || itemid == 1058 || itemid == 1094 || itemid == 1096 || itemid == 3402 || itemid == 1098 || itemid == 1828 || itemid == 3870 || itemid == 7058 || itemid == 1938 || itemid == 8452 || itemid == 1740 || itemid == 3040 || itemid == 5080 || itemid == 3100 || itemid == 1550 || itemid == 5740 || itemid == 3074 || itemid == 3010 || itemid == 8480 || itemid == 5084 || itemid == 10424 || itemid == 4656 || itemid == 7558 || itemid == 5082 || itemid == 1636 || itemid == 6008 || itemid == 4996 || itemid == 6416 || itemid == 2206 || itemid == 3792 || itemid == 3196 || itemid == 4654 || itemid == 3306 || itemid == 1498 || itemid == 1500 || itemid == 2804 || itemid == 2806 || itemid == 8270 || itemid == 8272 || itemid == 8274 || itemid == 2242 || itemid == 2244 || itemid == 2246 || itemid == 2248 || itemid == 2250 || itemid == 4676 || itemid == 4678 || itemid == 4680 || itemid == 4682 || itemid == 4652 || itemid == 4646 || itemid == 4648 || itemid == 4652 || itemid == 4650 || itemid == 10084 || itemid == 10086 || itemid == 9168 || itemid == 5480 || itemid == 4534 || itemid == 9166 || itemid == 9164 || itemid == 9162 || itemid == 9160 || itemid == 9158 || itemid == 9156 || itemid == 9154 || itemid == 9152 || itemid == 3008 || itemid == 3010 || itemid == 3040 || itemid == 5740 || itemid == 6254 || itemid == 6256 || itemid == 6258 || itemid == 6932 || itemid == 10262 || itemid == 10616 || itemid == 10582 || itemid == 10580 || itemid == 10664 || itemid == 10596 || itemid == 10598 || itemid == 10586 || itemid == 10590 || itemid == 10592 || itemid == 10576 || itemid == 10578 || itemid == 202 || itemid == 204 || itemid == 206 || itemid == 4994 || itemid == 2978 || itemid == 5766 || itemid == 5768 || itemid == 5744 || itemid == 5756 || itemid == 5758 || itemid == 5760 || itemid == 5762 || itemid == 5754 || itemid == 7688 || itemid == 7690 || itemid == 7694 || itemid == 7686 || itemid == 7692 || itemid == 7698 || itemid == 7696 || itemid == 9286 || itemid == 9272 || itemid == 9290 || itemid == 9280 || itemid == 9282 || itemid == 9292 || itemid == 9284 || itemid == 362 || itemid == 3398 || itemid == 386 || itemid == 4422 || itemid == 364 || itemid == 9340 || itemid == 9342 || itemid == 9332 || itemid == 9334 || itemid == 9336 || itemid == 9338 || itemid == 366 || itemid == 2388 || itemid == 7808 || itemid == 7810 || itemid == 4416 || itemid == 7818 || itemid == 7820 || itemid == 5652 || itemid == 7822 || itemid == 7824 || itemid == 5644 || itemid == 390 || itemid == 7826 || itemid == 7830 || itemid == 9324 || itemid == 5658 || itemid == 3396 || itemid == 2384 || itemid == 5660 || itemid == 3400 || itemid == 4418 || itemid == 4412 || itemid == 388 || itemid == 3408 || itemid == 1470 || itemid == 3404 || itemid == 3406 || itemid == 2390 || itemid == 5656 || itemid == 5648 || itemid == 2396 || itemid == 384 || itemid == 5664 || itemid == 4424 || itemid == 4400 || itemid == 1458 || itemid == 10660 || itemid == 10654 || itemid == 10632 || itemid == 10652 || itemid == 10626 || itemid == 10640 || itemid == 10662 || itemid == 574 || itemid == 592 || itemid == 760 || itemid == 900 || itemid == 766 || itemid == 1012 || itemid == 1272 || itemid == 1320 || itemid == 1540 || itemid == 1648 || itemid == 1740 || itemid == 1950 || itemid == 2900 || itemid == 1022 || itemid == 1030 || itemid == 1024 || itemid == 1032 || itemid == 1026 || itemid == 1028 || itemid == 1036 || itemid == 1034 || itemid == 2908 || itemid == 2974 || itemid == 3494 || itemid == 3060 || itemid == 3056 || itemid == 3052 || itemid == 3066 || itemid == 3048 || itemid == 3068 || itemid == 3166 || itemid == 2032 || itemid == 6780 || itemid == 754 || itemid == 794 || itemid == 796 || itemid == 2876 || itemid == 798 || itemid == 930 || itemid == 2204 || itemid == 2912 || itemid == 772 || itemid == 770 || itemid == 898 || itemid == 1582 || itemid == 1020 || itemid == 4132 || itemid == 3932 || itemid == 3934 || itemid == 4128 || itemid == 10246 || itemid == 4296 || itemid == 6212 || itemid == 1212 || itemid == 1190 || itemid == 1206 || itemid == 1166 || itemid == 1964 || itemid == 1976 || itemid == 1998 || itemid == 1946 || itemid == 2002 || itemid == 1958 || itemid == 1952 || itemid == 2030 || itemid == 3104 || itemid == 3112 || itemid == 3120 || itemid == 3092 || itemid == 3094 || itemid == 3096 || itemid == 4184 || itemid == 4178 || itemid == 4174 || itemid == 4180 || itemid == 4170 || itemid == 4168 || itemid == 4150 || itemid == 1180 || itemid == 1224 || itemid == 5226 || itemid == 5228 || itemid == 5230 || itemid == 5212 || itemid == 5246 || itemid == 5242 || itemid == 5234 || itemid == 7134 || itemid == 7118 || itemid == 7132 || itemid == 7120 || itemid == 7098 || itemid == 9018 || itemid == 9038 || itemid == 9026 || itemid == 9066 || itemid == 9058 || itemid == 9044 || itemid == 9024 || itemid == 9032 || itemid == 9036 || itemid == 9028 || itemid == 9030 || itemid == 9110 || itemid == 9112 || itemid == 10386 || itemid == 10326 || itemid == 10324 || itemid == 10322 || itemid == 10328 || itemid == 10316 || itemid == 1198 || itemid == 1208 || itemid == 1222 || itemid == 1200 || itemid == 1220 || itemid == 1202 || itemid == 1238 || itemid == 1168 || itemid == 1172 || itemid == 1230 || itemid == 1194 || itemid == 1192 || itemid == 1226 || itemid == 1196 || itemid == 1236 || itemid == 1182 || itemid == 1184 || itemid == 1186 || itemid == 1188 || itemid == 1170 || itemid == 1212 || itemid == 1214 || itemid == 1232 || itemid == 1178 || itemid == 1234 || itemid == 1250 || itemid == 1956 || itemid == 1990 || itemid == 1968 || itemid == 1960 || itemid == 1948 || itemid == 1966 || itemid == 3114 || itemid == 3118 || itemid == 3100 || itemid == 3122 || itemid == 3124 || itemid == 3126 || itemid == 3108 || itemid == 3098 || itemid == 1962 || itemid == 2000 || itemid == 4186 || itemid == 4188 || itemid == 4246 || itemid == 4248 || itemid == 4192 || itemid == 4156 || itemid == 4136 || itemid == 4152 || itemid == 4166 || itemid == 4190 || itemid == 4172 || itemid == 4182 || itemid == 4144 || itemid == 4146 || itemid == 4148 || itemid == 4140 || itemid == 4138 || itemid == 4142 || itemid == 5256 || itemid == 5208 || itemid == 5216 || itemid == 5218 || itemid == 5220 || itemid == 5214 || itemid == 5210 || itemid == 5254 || itemid == 5250 || itemid == 5252 || itemid == 5244 || itemid == 5236 || itemid == 7104 || itemid == 7124 || itemid == 7122 || itemid == 7102 || itemid == 7100 || itemid == 7126 || itemid == 7104 || itemid == 7124 || itemid == 7122 || itemid == 7102 || itemid == 7100 || itemid == 9048 || itemid == 9056 || itemid == 9034 || itemid == 1210 || itemid == 1216 || itemid == 1218 || itemid == 1992 || itemid == 1982 || itemid == 1994 || itemid == 1972 || itemid == 1980 || itemid == 1988 || itemid == 1984 || itemid == 3116 || itemid == 3102 || itemid == 3106 || itemid == 3110 || itemid == 4160 || itemid == 4162 || itemid == 4164 || itemid == 4154 || itemid == 4158 || itemid == 5224 || itemid == 5222 || itemid == 5232 || itemid == 5240 || itemid == 5248 || itemid == 5238 || itemid == 5256 || itemid == 7116 || itemid == 7108 || itemid == 7110 || itemid == 7128 || itemid == 7112 || itemid == 7130) break;
												if (itemid == 6398 || itemid == 6426 || itemid == 6340 || itemid == 6342 || itemid == 6350 || itemid == 6818 || itemid == 8244 || itemid == 8242 || itemid == 8240 || itemid == 8452 || itemid == 8454 || itemid == 8488 || itemid == 8498 || itemid == 8474 || itemid == 8476 || itemid == 8492 || itemid == 1498 || itemid == 1500 || itemid == 2804 || itemid == 2806 || itemid == 8270 || itemid == 8272 || itemid == 8274 || itemid == 3172 || itemid == 8478 || itemid == 8480 || itemid == 8486 || itemid == 8484 || itemid == 8482 || itemid == 8468 || itemid == 8494 || itemid == 8466 || itemid == 8490 || itemid == 8456 || itemid == 8458 || itemid == 8496 || itemid == 8472 || itemid == 5482 || itemid == 2240 || itemid == 3204 || itemid == 6114 || itemid == 4328 || itemid == 4326 || itemid == 4330 || itemid == 4324 || itemid == 4334 || itemid == 1242 || itemid == 1244 || itemid == 1246 || itemid == 1248 || itemid == 1282 || itemid == 1284 || itemid == 1286 || itemid == 1290 || itemid == 1288 || itemid == 1292 || itemid == 1294 || itemid == 1256 || itemid == 2586 || itemid == 782 || itemid == 3536 || itemid == 764 || itemid == 4176 || itemid == 4322 || itemid == 4080 || itemid == 2992 || itemid == 2976 || itemid == 3790 || itemid == 4990 || itemid == 1506 || itemid == 1274 || itemid == 9000 || itemid == 1252 || itemid == 8284 || itemid == 8954 || itemid == 8534 || itemid == 2386 || itemid == 4428 || itemid == 4426 || itemid == 5662 || itemid == 5642 || itemid == 5654 || itemid == 5646 || itemid == 5650 || itemid == 7828 || itemid == 7832 || itemid == 7834 || itemid == 9322 || itemid == 9344 || itemid == 9326 || itemid == 9316 || itemid == 9318 || itemid == 362 || itemid == 3398 || itemid == 386 || itemid == 4422 || itemid == 364 || itemid == 9340 || itemid == 9342 || itemid == 9332 || itemid == 9334 || itemid == 9336 || itemid == 9338 || itemid == 366 || itemid == 2388 || itemid == 7808 || itemid == 7810 || itemid == 4416 || itemid == 7818 || itemid == 7820 || itemid == 5652 || itemid == 7822 || itemid == 7824 || itemid == 5644 || itemid == 390 || itemid == 7826 || itemid == 7830 || itemid == 9324 || itemid == 5658 || itemid == 3396 || itemid == 2384 || itemid == 5660 || itemid == 3400 || itemid == 4418 || itemid == 4412 || itemid == 388 || itemid == 3408 || itemid == 1470 || itemid == 3404 || itemid == 3406 || itemid == 2390 || itemid == 5656 || itemid == 5648 || itemid == 2396 || itemid == 384 || itemid == 5664 || itemid == 4424 || itemid == 4400 || itemid == 9350 || itemid == 5040 || itemid == 5042 || itemid == 5044 || itemid == 392 || itemid == 3402 || itemid == 5032 || itemid == 5034 || itemid == 5036 || itemid == 5038 || itemid == 5018 || itemid == 5022 || itemid == 5060 || itemid == 5054 || itemid == 5058 || itemid == 5056 || itemid == 5050 || itemid == 5046 || itemid == 5052 || itemid == 5048 || itemid == 5070 || itemid == 5072 || itemid == 5074 || itemid == 5076 || itemid == 5066 || itemid == 5062 || itemid == 5068 || itemid == 5064 || itemid == 5080 || itemid == 5082 || itemid == 5084 || itemid == 5078 || itemid == 10236 || itemid == 10232 || itemid == 10194 || itemid == 10206 || itemid == 10184 || itemid == 10192 || itemid == 10190 || itemid == 10186 || itemid == 10212 || itemid == 10214 || itemid == 10216 || itemid == 10220 || itemid == 10222 || itemid == 10224 || itemid == 10226 || itemid == 10208 || itemid == 10210 || itemid == 10218 || itemid == 10196 || itemid == 10198 || itemid == 10200 || itemid == 10202 || itemid == 10204) break;
												int kiek_turi_kainoti = 0;
												try {
													ifstream infile("config+/price_data.txt");
													for (string line; getline(infile, line);) {
														if (line.length() > 3 && line.at(0) != '/' && line.at(1) != '/') {
															auto ex = explode("|", line);
															if (ex.at(0) == to_string(item_id)) {
																kiek_turi_kainoti = atoi(ex.at(1).c_str());
																break;
															}
														}
													}
													infile.close();
												}
												catch (const std::out_of_range& e) {
													std::cout << e.what() << std::endl;
												}
												if (itemid == 7188) kiek_turi_kainoti = 10000;
												int kiek_ruosiasi_parduoti = 0;
												if (getItemDef(item_id).blockType == BlockTypes::SIGN && getItemDef(item_id).rarity != 999 || getItemDef(item_id).blockType == BlockTypes::GATEWAY && getItemDef(item_id).rarity != 999 || getItemDef(item_id).blockType == BlockTypes::BACKGROUND && getItemDef(item_id).rarity != 999 || getItemDef(item_id).blockType == BlockTypes::DOOR && getItemDef(item_id).rarity != 999 || getItemDef(item_id).blockType == BlockTypes::DEADLY && getItemDef(item_id).rarity != 999 || getItemDef(item_id).blockType == BlockTypes::PAIN_BLOCK && getItemDef(item_id).rarity != 999 || getItemDef(item_id).blockType == BlockTypes::PLATFORM && getItemDef(item_id).rarity != 999 || getItemDef(item_id).blockType == BlockTypes::FOREGROUND && getItemDef(item_id).rarity != 999 && item_id != 9498 && item_id != 5154 && item_id != 5140 && item_id != 5138 && item_id != 5136 && item_id != 7382 && item_id != 4762 || getItemDef(item_id).blockType == BlockTypes::SFX_FOREGROUND && getItemDef(item_id).rarity != 999 || getItemDef(item_id).blockType == BlockTypes::ANIM_FOREGROUND && getItemDef(item_id).rarity != 999 || getItemDef(item_id).blockType == BlockTypes::TOGGLE_FOREGROUND && getItemDef(item_id).rarity != 999) {
													if (getItemDef(item_id).rarity < 14) break;
													kiek_ruosiasi_parduoti = item_count * kiek_turi_kainoti;
												}
												else {
													if (getItemDef(item_id).rarity != 999 && itemid != 9498 && itemid != 5154 && itemid != 5140 && itemid != 5138 && itemid != 5136 && itemid != 7382 && itemid != 4762) break;
													kiek_ruosiasi_parduoti = item_count;
												}
												if (kiek_ruosiasi_parduoti < 1 || kiek_ruosiasi_parduoti > 250) {
													Player::OnTalkBubble(peer, pData->netID, "You can't sell more than 250 or less than 0!", 0, true);
													break;
												}
												if (item_id == 9498 || item_id == 5154 || item_id == 5140 || item_id == 5138 || item_id == 5136 || item_id == 7382 || item_id == 4762) {
													kiek_ruosiasi_parduoti = 200;
												}
												auto contains = false;
												SearchInventoryItem(peer, item_id, kiek_ruosiasi_parduoti, contains);
												if (!contains) {
													Player::OnTalkBubble(peer, pData->netID, "You don't have " + to_string(kiek_ruosiasi_parduoti) + " of " + getItemDef(item_id).name + " to sell!", 0, true);
													break;
												}
												auto worldlocks = 0;
												if (item_id == 9498 || item_id == 5154 || item_id == 5140 || item_id == 5138 || item_id == 5136 || item_id == 7382 || item_id == 4762) {
													worldlocks = kiek_turi_kainoti;
												}
												else if (getItemDef(item_id).blockType == BlockTypes::SIGN && getItemDef(item_id).rarity != 999 || getItemDef(item_id).blockType == BlockTypes::GATEWAY && getItemDef(item_id).rarity != 999 || getItemDef(item_id).blockType == BlockTypes::BACKGROUND && getItemDef(item_id).rarity != 999 || getItemDef(item_id).blockType == BlockTypes::DOOR && getItemDef(item_id).rarity != 999 || getItemDef(item_id).blockType == BlockTypes::DEADLY && getItemDef(item_id).rarity != 999 || getItemDef(item_id).blockType == BlockTypes::PAIN_BLOCK && getItemDef(item_id).rarity != 999 || getItemDef(item_id).blockType == BlockTypes::PLATFORM && getItemDef(item_id).rarity != 999 || getItemDef(item_id).blockType == BlockTypes::FOREGROUND && getItemDef(item_id).rarity != 999 && item_id != 9498 && item_id != 5154 && item_id != 5140 && item_id != 5138 && item_id != 5136 && item_id != 7382 && item_id != 4762 || getItemDef(item_id).blockType == BlockTypes::SFX_FOREGROUND && getItemDef(item_id).rarity != 999 || getItemDef(item_id).blockType == BlockTypes::ANIM_FOREGROUND && getItemDef(item_id).rarity != 999 || getItemDef(item_id).blockType == BlockTypes::TOGGLE_FOREGROUND && getItemDef(item_id).rarity != 999) {
													worldlocks = item_count;
												}
												else {
													worldlocks = kiek_turi_kainoti * kiek_ruosiasi_parduoti;
												}
												auto diamondlocks = 0;
												while (worldlocks >= 100) {
													diamondlocks++;
													worldlocks -= 100;
												}
												if (CheckItemMaxed(peer, 242, worldlocks) || worldlocks > 250) {
													Player::OnTalkBubble(peer, pData->netID, "" + to_string(worldlocks) + " World locks Wouldnt fit into my inventory!", 0, true);
													break;
												}
												if (CheckItemMaxed(peer, 1796, diamondlocks) || diamondlocks > 250) {
													Player::OnTalkBubble(peer, pData->netID, "" + to_string(diamondlocks) + " Diamond locks Wouldnt fit into my inventory!", 0, true);
													break;
												}
												string kiek_gaus = "";
												if (diamondlocks != 0) {
													kiek_gaus += "\nadd_label_with_icon|small|(`w" + to_string(diamondlocks) + "``) `2Diamond Locks``|left|1796|";
												}
												if (worldlocks != 0) {
													kiek_gaus += "\nadd_label_with_icon|small|(`w" + to_string(worldlocks) + "``) `2World Locks``|left|242|";
												}
												pData->laststoresellworldlocks = worldlocks;
												pData->laststoreselldiamondlocks = diamondlocks;
												pData->laststoresellid = item_id;
												pData->laststoresellcount = kiek_ruosiasi_parduoti;
												Player::OnDialogRequest(peer, "set_default_color|`o\nadd_label_with_icon|big|`wPurchase Confirmation``|left|1366|\nadd_spacer|small|\nadd_spacer|small|\nadd_textbox|`4You'll give:``|left|\nadd_spacer|small|\nadd_label_with_icon|small|(`w" + to_string(kiek_ruosiasi_parduoti) + "``) `8" + getItemDef(item_id).name + "``|left|" + to_string(item_id) + "|\nadd_spacer|small|\nadd_textbox|`2You'll get:``|left|\nadd_spacer|small|" + kiek_gaus + "\nadd_spacer|small|\nadd_spacer|small|\nadd_textbox|Are you sure you want to make this purchase?|left|\nend_dialog|store_sell_confirm|Cancel|OK|");
												break;
											}
											bool contains_non_int2 = !std::regex_match(strButItemByRarityCount, std::regex("^[0-9]+$"));
											bool contains_non_int4 = !std::regex_match(id, std::regex("^[0-9]+$"));
											if (strButItemByRarityCount == "" || contains_non_int4 || id.size() > 5 || id.size() <= 0 || strButItemByRarityCount.length() > 5 || contains_non_int2 || strButItemByRarityCount.size() > 3 || strButItemByRarityCount.size() <= 0) {
												Player::OnTalkBubble(peer, pData->netID, "You can't buy more than 250 or less than 0!", 0, true);
												break;
											}
											int item_count = atoi(strButItemByRarityCount.c_str());
											if (item_count < 1 || item_count > 250) {
												Player::OnTalkBubble(peer, pData->netID, "You can't buy more than 250 or less than 0!", 0, true);
												break;
											}
											int item_id = atoi(id.c_str());
											int itemid = item_id;
											if (getItemDef(itemid).blockType != BlockTypes::CLOTHING && getItemDef(itemid).rarity == 999 && itemid != 7188 && itemid != 5142 || getItemDef(itemid).blockType == BlockTypes::LOCK && itemid != 7188 || itemid == 10034 || getItemDef(itemid).name.find("null") != string::npos || itemid == 10036 || getItemDef(itemid).name.find("Mooncake") != string::npos || getItemDef(itemid).name.find("Harvest") != string::npos && itemid != 1830 || getItemDef(itemid).name.find("Autumn") != string::npos || itemid == 1056 || itemid == 1804 || getItemDef(itemid).blockType == BlockTypes::COMPONENT || getItemDef(itemid).properties & Property_Chemical || itemid == 6920 || itemid == 6922 || itemid == 1874 || itemid == 1876 || itemid == 1904 || itemid == 1932 || itemid == 1900 || itemid == 1986 || itemid == 1996 || itemid == 2970 || itemid == 3140 || itemid == 3174 || itemid == 6028 || itemid == 6846 || itemid == 8962 || itemid == 2408 || itemid == 4428 || itemid == 5086 || itemid == 9240 || itemid == 9306 || itemid == 9290 || itemid == 7328 || itemid == 9416 || itemid == 10386 || itemid == 9410 || itemid == 1458 || itemid == 9408 || itemid == 9360 || itemid == 6866 || itemid == 6868 || itemid == 6870 || itemid == 6872 || itemid == 6874 || itemid == 6876 || itemid == 6878 || itemid == 2480 || itemid == 8452 || itemid == 5132 || itemid == 7166 || itemid == 5126 || itemid == 5128 || itemid == 5130 || itemid == 5144 || itemid == 5146 || itemid == 5148 || itemid == 5150 || itemid == 5162 || itemid == 5164 || itemid == 5166 || itemid == 5168 || itemid == 5180 || itemid == 5182 || itemid == 5184 || itemid == 5186 || itemid == 7168 || itemid == 7170 || itemid == 7172 || itemid == 7174 || itemid == 8834 || itemid == 7912 || itemid == 9212 || itemid == 5134 || itemid == 5152 || itemid == 5170 || itemid == 5188 || itemid == 980 || itemid == 9448 || itemid == 9310 || itemid == 10034 || itemid == 10036 || itemid == 8470 || itemid == 8286 || itemid == 6026 || itemid == 1784 || itemid == 9356 || itemid == 10022 || itemid == 902 || itemid == 10032 || itemid == 834 || itemid == 6 || itemid == 5640 || itemid == 9492 || itemid == 1782 || itemid == 9288 || itemid == 1780 || itemid == 8306 || itemid == 202 || itemid == 204 || itemid == 206 || itemid == 2950 || itemid == 4802 || itemid == 4994 || itemid == 5260 || itemid == 5814 || itemid == 5980 || itemid == 7734 || itemid == 2592 || itemid == 2242 || itemid == 1794 || itemid == 1792 || itemid == 778 || itemid == 9510 || itemid == 8774 || itemid == 2568 || itemid == 9512 || itemid == 9502 || itemid == 9482 || itemid == 2250 || itemid == 2248 || itemid == 2244 || itemid == 2246 || itemid == 2286 || itemid == 9508 || itemid == 9504 || itemid == 9506 || itemid == 274 || itemid == 276 || itemid == 9476 || itemid == 1486 || itemid == 4426 || itemid == 9496 || itemid == 278 || itemid == 9490 || itemid == 2410 || itemid == 9488 || itemid == 9452 || itemid == 9454 || itemid == 9472 || itemid == 9456 || itemid == 732 || itemid == 9458 || itemid == 6336 || itemid == 112 || itemid == 8 || itemid == 3760 || getItemDef(itemid).blockType == BlockTypes::FISH || itemid == 7372 || itemid == 9438 || itemid == 9462 || itemid == 9440 || itemid == 9442 || itemid == 9444 || itemid == 7960 || itemid == 7628 || itemid == 8552) break;
											if (itemid == 6312 || itemid == 998 || itemid == 986 || itemid == 992 || itemid == 990 || itemid == 996 || itemid == 988 || itemid == 1004 || itemid == 1006 || itemid == 1002 || itemid == 9504 || itemid == 9506 || itemid == 9508 || itemid == 9510 || itemid == 9512 || itemid == 9514 || itemid == 9518 || itemid == 9520 || itemid == 9502 || itemid == 9496 || itemid == 1790 || itemid == 9492 || itemid == 9488 || itemid == 9222 || itemid == 1360 || itemid == 6260 || itemid == 822 || itemid == 1058 || itemid == 1094 || itemid == 1096 || itemid == 3402 || itemid == 1098 || itemid == 1828 || itemid == 3870 || itemid == 7058 || itemid == 1938 || itemid == 8452 || itemid == 1740 || itemid == 3040 || itemid == 5080 || itemid == 3100 || itemid == 1550 || itemid == 5740 || itemid == 3074 || itemid == 3010 || itemid == 8480 || itemid == 5084 || itemid == 10424 || itemid == 4656 || itemid == 7558 || itemid == 5082 || itemid == 1636 || itemid == 6008 || itemid == 4996 || itemid == 6416 || itemid == 2206 || itemid == 3792 || itemid == 3196 || itemid == 4654 || itemid == 3306 || itemid == 1498 || itemid == 1500 || itemid == 2804 || itemid == 2806 || itemid == 8270 || itemid == 8272 || itemid == 8274 || itemid == 2242 || itemid == 2244 || itemid == 2246 || itemid == 2248 || itemid == 2250 || itemid == 4676 || itemid == 4678 || itemid == 4680 || itemid == 4682 || itemid == 4652 || itemid == 4646 || itemid == 4648 || itemid == 4652 || itemid == 4650 || itemid == 10084 || itemid == 10086 || itemid == 9168 || itemid == 5480 || itemid == 4534 || itemid == 9166 || itemid == 9164 || itemid == 9162 || itemid == 9160 || itemid == 9158 || itemid == 9156 || itemid == 9154 || itemid == 9152 || itemid == 3008 || itemid == 3010 || itemid == 3040 || itemid == 5740 || itemid == 6254 || itemid == 6256 || itemid == 6258 || itemid == 6932 || itemid == 10262 || itemid == 10616 || itemid == 10582 || itemid == 10580 || itemid == 10664 || itemid == 10596 || itemid == 10598 || itemid == 10586 || itemid == 10590 || itemid == 10592 || itemid == 10576 || itemid == 10578 || itemid == 202 || itemid == 204 || itemid == 206 || itemid == 4994 || itemid == 2978 || itemid == 5766 || itemid == 5768 || itemid == 5744 || itemid == 5756 || itemid == 5758 || itemid == 5760 || itemid == 5762 || itemid == 5754 || itemid == 7688 || itemid == 7690 || itemid == 7694 || itemid == 7686 || itemid == 7692 || itemid == 7698 || itemid == 7696 || itemid == 9286 || itemid == 9272 || itemid == 9290 || itemid == 9280 || itemid == 9282 || itemid == 9292 || itemid == 9284 || itemid == 362 || itemid == 3398 || itemid == 386 || itemid == 4422 || itemid == 364 || itemid == 9340 || itemid == 9342 || itemid == 9332 || itemid == 9334 || itemid == 9336 || itemid == 9338 || itemid == 366 || itemid == 2388 || itemid == 7808 || itemid == 7810 || itemid == 4416 || itemid == 7818 || itemid == 7820 || itemid == 5652 || itemid == 7822 || itemid == 7824 || itemid == 5644 || itemid == 390 || itemid == 7826 || itemid == 7830 || itemid == 9324 || itemid == 5658 || itemid == 3396 || itemid == 2384 || itemid == 5660 || itemid == 3400 || itemid == 4418 || itemid == 4412 || itemid == 388 || itemid == 3408 || itemid == 1470 || itemid == 3404 || itemid == 3406 || itemid == 2390 || itemid == 5656 || itemid == 5648 || itemid == 2396 || itemid == 384 || itemid == 5664 || itemid == 4424 || itemid == 4400 || itemid == 1458 || itemid == 10660 || itemid == 10654 || itemid == 10632 || itemid == 10652 || itemid == 10626 || itemid == 10640 || itemid == 10662 || itemid == 574 || itemid == 592 || itemid == 760 || itemid == 900 || itemid == 766 || itemid == 1012 || itemid == 1272 || itemid == 1320 || itemid == 1540 || itemid == 1648 || itemid == 1740 || itemid == 1950 || itemid == 2900 || itemid == 1022 || itemid == 1030 || itemid == 1024 || itemid == 1032 || itemid == 1026 || itemid == 1028 || itemid == 1036 || itemid == 1034 || itemid == 2908 || itemid == 2974 || itemid == 3494 || itemid == 3060 || itemid == 3056 || itemid == 3052 || itemid == 3066 || itemid == 3048 || itemid == 3068 || itemid == 3166 || itemid == 2032 || itemid == 6780 || itemid == 754 || itemid == 794 || itemid == 796 || itemid == 2876 || itemid == 798 || itemid == 930 || itemid == 2204 || itemid == 2912 || itemid == 772 || itemid == 770 || itemid == 898 || itemid == 1582 || itemid == 1020 || itemid == 4132 || itemid == 3932 || itemid == 3934 || itemid == 4128 || itemid == 10246 || itemid == 4296 || itemid == 6212 || itemid == 1212 || itemid == 1190 || itemid == 1206 || itemid == 1166 || itemid == 1964 || itemid == 1976 || itemid == 1998 || itemid == 1946 || itemid == 2002 || itemid == 1958 || itemid == 1952 || itemid == 2030 || itemid == 3104 || itemid == 3112 || itemid == 3120 || itemid == 3092 || itemid == 3094 || itemid == 3096 || itemid == 4184 || itemid == 4178 || itemid == 4174 || itemid == 4180 || itemid == 4170 || itemid == 4168 || itemid == 4150 || itemid == 1180 || itemid == 1224 || itemid == 5226 || itemid == 5228 || itemid == 5230 || itemid == 5212 || itemid == 5246 || itemid == 5242 || itemid == 5234 || itemid == 7134 || itemid == 7118 || itemid == 7132 || itemid == 7120 || itemid == 7098 || itemid == 9018 || itemid == 9038 || itemid == 9026 || itemid == 9066 || itemid == 9058 || itemid == 9044 || itemid == 9024 || itemid == 9032 || itemid == 9036 || itemid == 9028 || itemid == 9030 || itemid == 9110 || itemid == 9112 || itemid == 10386 || itemid == 10326 || itemid == 10324 || itemid == 10322 || itemid == 10328 || itemid == 10316 || itemid == 1198 || itemid == 1208 || itemid == 1222 || itemid == 1200 || itemid == 1220 || itemid == 1202 || itemid == 1238 || itemid == 1168 || itemid == 1172 || itemid == 1230 || itemid == 1194 || itemid == 1192 || itemid == 1226 || itemid == 1196 || itemid == 1236 || itemid == 1182 || itemid == 1184 || itemid == 1186 || itemid == 1188 || itemid == 1170 || itemid == 1212 || itemid == 1214 || itemid == 1232 || itemid == 1178 || itemid == 1234 || itemid == 1250 || itemid == 1956 || itemid == 1990 || itemid == 1968 || itemid == 1960 || itemid == 1948 || itemid == 1966 || itemid == 3114 || itemid == 3118 || itemid == 3100 || itemid == 3122 || itemid == 3124 || itemid == 3126 || itemid == 3108 || itemid == 3098 || itemid == 1962 || itemid == 2000 || itemid == 4186 || itemid == 4188 || itemid == 4246 || itemid == 4248 || itemid == 4192 || itemid == 4156 || itemid == 4136 || itemid == 4152 || itemid == 4166 || itemid == 4190 || itemid == 4172 || itemid == 4182 || itemid == 4144 || itemid == 4146 || itemid == 4148 || itemid == 4140 || itemid == 4138 || itemid == 4142 || itemid == 5256 || itemid == 5208 || itemid == 5216 || itemid == 5218 || itemid == 5220 || itemid == 5214 || itemid == 5210 || itemid == 5254 || itemid == 5250 || itemid == 5252 || itemid == 5244 || itemid == 5236 || itemid == 7104 || itemid == 7124 || itemid == 7122 || itemid == 7102 || itemid == 7100 || itemid == 7126 || itemid == 7104 || itemid == 7124 || itemid == 7122 || itemid == 7102 || itemid == 7100 || itemid == 9048 || itemid == 9056 || itemid == 9034 || itemid == 1210 || itemid == 1216 || itemid == 1218 || itemid == 1992 || itemid == 1982 || itemid == 1994 || itemid == 1972 || itemid == 1980 || itemid == 1988 || itemid == 1984 || itemid == 3116 || itemid == 3102 || itemid == 3106 || itemid == 3110 || itemid == 4160 || itemid == 4162 || itemid == 4164 || itemid == 4154 || itemid == 4158 || itemid == 5224 || itemid == 5222 || itemid == 5232 || itemid == 5240 || itemid == 5248 || itemid == 5238 || itemid == 5256 || itemid == 7116 || itemid == 7108 || itemid == 7110 || itemid == 7128 || itemid == 7112 || itemid == 7130) break;
											if (itemid == 6398 || itemid == 6426 || itemid == 6340 || itemid == 6342 || itemid == 6350 || itemid == 6818 || itemid == 8244 || itemid == 8242 || itemid == 8240 || itemid == 8452 || itemid == 8454 || itemid == 8488 || itemid == 8498 || itemid == 8474 || itemid == 8476 || itemid == 8492 || itemid == 1498 || itemid == 1500 || itemid == 2804 || itemid == 2806 || itemid == 8270 || itemid == 8272 || itemid == 8274 || itemid == 3172 || itemid == 8478 || itemid == 8480 || itemid == 8486 || itemid == 8484 || itemid == 8482 || itemid == 8468 || itemid == 8494 || itemid == 8466 || itemid == 8490 || itemid == 8456 || itemid == 8458 || itemid == 8496 || itemid == 8472 || itemid == 5482 || itemid == 2240 || itemid == 3204 || itemid == 6114 || itemid == 4328 || itemid == 4326 || itemid == 4330 || itemid == 4324 || itemid == 4334 || itemid == 1242 || itemid == 1244 || itemid == 1246 || itemid == 1248 || itemid == 1282 || itemid == 1284 || itemid == 1286 || itemid == 1290 || itemid == 1288 || itemid == 1292 || itemid == 1294 || itemid == 1256 || itemid == 2586 || itemid == 782 || itemid == 3536 || itemid == 764 || itemid == 4176 || itemid == 4322 || itemid == 4080 || itemid == 2992 || itemid == 2976 || itemid == 3790 || itemid == 4990 || itemid == 1506 || itemid == 1274 || itemid == 9000 || itemid == 1252 || itemid == 8284 || itemid == 8954 || itemid == 8534 || itemid == 2386 || itemid == 4428 || itemid == 4426 || itemid == 5662 || itemid == 5642 || itemid == 5654 || itemid == 5646 || itemid == 5650 || itemid == 7828 || itemid == 7832 || itemid == 7834 || itemid == 9322 || itemid == 9344 || itemid == 9326 || itemid == 9316 || itemid == 9318 || itemid == 362 || itemid == 3398 || itemid == 386 || itemid == 4422 || itemid == 364 || itemid == 9340 || itemid == 9342 || itemid == 9332 || itemid == 9334 || itemid == 9336 || itemid == 9338 || itemid == 366 || itemid == 2388 || itemid == 7808 || itemid == 7810 || itemid == 4416 || itemid == 7818 || itemid == 7820 || itemid == 5652 || itemid == 7822 || itemid == 7824 || itemid == 5644 || itemid == 390 || itemid == 7826 || itemid == 7830 || itemid == 9324 || itemid == 5658 || itemid == 3396 || itemid == 2384 || itemid == 5660 || itemid == 3400 || itemid == 4418 || itemid == 4412 || itemid == 388 || itemid == 3408 || itemid == 1470 || itemid == 3404 || itemid == 3406 || itemid == 2390 || itemid == 5656 || itemid == 5648 || itemid == 2396 || itemid == 384 || itemid == 5664 || itemid == 4424 || itemid == 4400 || itemid == 9350 || itemid == 5040 || itemid == 5042 || itemid == 5044 || itemid == 392 || itemid == 3402 || itemid == 5032 || itemid == 5034 || itemid == 5036 || itemid == 5038 || itemid == 5018 || itemid == 5022 || itemid == 5060 || itemid == 5054 || itemid == 5058 || itemid == 5056 || itemid == 5050 || itemid == 5046 || itemid == 5052 || itemid == 5048 || itemid == 5070 || itemid == 5072 || itemid == 5074 || itemid == 5076 || itemid == 5066 || itemid == 5062 || itemid == 5068 || itemid == 5064 || itemid == 5080 || itemid == 5082 || itemid == 5084 || itemid == 5078 || itemid == 10236 || itemid == 10232 || itemid == 10194 || itemid == 10206 || itemid == 10184 || itemid == 10192 || itemid == 10190 || itemid == 10186 || itemid == 10212 || itemid == 10214 || itemid == 10216 || itemid == 10220 || itemid == 10222 || itemid == 10224 || itemid == 10226 || itemid == 10208 || itemid == 10210 || itemid == 10218 || itemid == 10196 || itemid == 10198 || itemid == 10200 || itemid == 10202 || itemid == 10204) break;
											int kiek_turi_kainoti = 0;
											try {
												ifstream infile("config+/price_data.txt");
												for (string line; getline(infile, line);) {
													if (line.length() > 3 && line.at(0) != '/' && line.at(1) != '/') {
														auto ex = explode("|", line);
														if (ex.at(0) == to_string(item_id)) {
															kiek_turi_kainoti = atoi(ex.at(1).c_str());
															break;
														}
													}
												}
												infile.close();
											}
											catch (const std::out_of_range& e) {
												std::cout << e.what() << std::endl;
											}
											if (item_id == 7188) kiek_turi_kainoti = 10000;
											kiek_turi_kainoti *= item_count;
											int item_per_wl = 0;
											int kaina_wls = 0;
											if (getItemDef(item_id).blockType == BlockTypes::SIGN && getItemDef(item_id).rarity != 999 || getItemDef(item_id).blockType == BlockTypes::GATEWAY && getItemDef(item_id).rarity != 999 || getItemDef(item_id).blockType == BlockTypes::DEADLY && getItemDef(item_id).rarity != 999 || getItemDef(item_id).blockType == BlockTypes::DOOR && getItemDef(item_id).rarity != 999 || getItemDef(item_id).blockType == BlockTypes::PAIN_BLOCK && getItemDef(item_id).rarity != 999 || getItemDef(item_id).blockType == BlockTypes::PLATFORM && getItemDef(item_id).rarity != 999 || getItemDef(item_id).blockType == BlockTypes::BACKGROUND && getItemDef(item_id).rarity != 999 || getItemDef(item_id).blockType == BlockTypes::FOREGROUND && getItemDef(item_id).rarity != 999 && item_id != 9498 && item_id != 5154 && item_id != 5140 && item_id != 5138 && item_id != 5136 && item_id != 7382 && item_id != 4762 || getItemDef(item_id).blockType == BlockTypes::SFX_FOREGROUND && getItemDef(item_id).rarity != 999 || getItemDef(item_id).blockType == BlockTypes::ANIM_FOREGROUND && getItemDef(item_id).rarity != 999 || getItemDef(item_id).blockType == BlockTypes::TOGGLE_FOREGROUND && getItemDef(item_id).rarity != 999) {
												/*kiek per wl*/
												item_per_wl = kiek_turi_kainoti / item_count;
												/*itemperwl tai yra wl uz tiek*/
												kaina_wls = kiek_turi_kainoti / item_per_wl;
												/*kainawls tai yra tiek kiek turi moketi*/
											}
											else {
												/*kiek uz wl*/
												item_per_wl = kiek_turi_kainoti / item_count;
												/*itemperwl tai yra wl uz tiek*/
												kaina_wls = kiek_turi_kainoti;
												/*kainawls tai yra tiek kiek turi moketi*/
											}
											if (item_id == 7188) {
												item_per_wl = 1 * item_count;
												/*itemperwl tai yra wl uz tiek*/
												kaina_wls = kiek_turi_kainoti;
											}
											auto worldlocks = 0;
											auto diamondlocks = 0;
											auto locks = 0;
											for (auto i = 0; i < pData->inventory.items.size(); i++) {
												if (pData->inventory.items.at(i).itemID == 242) {
													worldlocks = pData->inventory.items.at(i).itemCount;
												}
												if (pData->inventory.items.at(i).itemID == 1796) {
													diamondlocks = pData->inventory.items.at(i).itemCount;
												}
											}
											if (diamondlocks > 0) locks = diamondlocks * 100;
											locks += worldlocks;
											if (kaina_wls > locks) {
												Player::OnTalkBubble(peer, pData->netID, "You can't afford that many!", 0, true);
												break;
											}
											if (getItemDef(item_id).blockType == BlockTypes::SIGN && getItemDef(item_id).rarity != 999 || getItemDef(item_id).blockType == BlockTypes::GATEWAY && getItemDef(item_id).rarity != 999 || getItemDef(item_id).blockType == BlockTypes::DEADLY && getItemDef(item_id).rarity != 999 || getItemDef(item_id).blockType == BlockTypes::DOOR && getItemDef(item_id).rarity != 999 || getItemDef(item_id).blockType == BlockTypes::PAIN_BLOCK && getItemDef(item_id).rarity != 999 || getItemDef(item_id).blockType == BlockTypes::PLATFORM && getItemDef(item_id).rarity != 999 || getItemDef(item_id).blockType == BlockTypes::BACKGROUND && getItemDef(item_id).rarity != 999 || getItemDef(item_id).blockType == BlockTypes::FOREGROUND && getItemDef(item_id).rarity != 999 && item_id != 9498 && item_id != 5154 && item_id != 5140 && item_id != 5138 && item_id != 5136 && item_id != 7382 && item_id != 4762 || getItemDef(item_id).blockType == BlockTypes::SFX_FOREGROUND && getItemDef(item_id).rarity != 999 || getItemDef(item_id).blockType == BlockTypes::ANIM_FOREGROUND && getItemDef(item_id).rarity != 999 || getItemDef(item_id).blockType == BlockTypes::TOGGLE_FOREGROUND && getItemDef(item_id).rarity != 999) {
												if (CheckItemMaxed(peer, item_id, kiek_turi_kainoti) || kiek_turi_kainoti > 250) {
													Player::OnTalkBubble(peer, pData->netID, "That wouldnt fit into my inventory!", 0, true);
													break;
												}
											}
											else {
												if (CheckItemMaxed(peer, item_id, item_count)) {
													Player::OnTalkBubble(peer, pData->netID, "That wouldnt fit into my inventory!", 0, true);
													break;
												}
											}
											if (item_id == 9498 || item_id == 5154 || item_id == 5140 || item_id == 5138 || item_id == 5136 || item_id == 7382 || item_id == 4762) {
												if (CheckItemMaxed(peer, item_id, 200)) {
													Player::OnTalkBubble(peer, pData->netID, "That wouldnt fit into my inventory!", 0, true);
													break;
												}
											}
											pData->laststorebuyprice = kaina_wls;
											pData->laststorebuyid = item_id;
											if (getItemDef(item_id).blockType == BlockTypes::SIGN && getItemDef(item_id).rarity != 999 || getItemDef(item_id).blockType == BlockTypes::GATEWAY && getItemDef(item_id).rarity != 999 || getItemDef(item_id).blockType == BlockTypes::DEADLY && getItemDef(item_id).rarity != 999 || getItemDef(item_id).blockType == BlockTypes::DOOR && getItemDef(item_id).rarity != 999 || getItemDef(item_id).blockType == BlockTypes::PAIN_BLOCK && getItemDef(item_id).rarity != 999 || getItemDef(item_id).blockType == BlockTypes::PLATFORM && getItemDef(item_id).rarity != 999 || getItemDef(item_id).blockType == BlockTypes::BACKGROUND && getItemDef(item_id).rarity != 999 || getItemDef(item_id).blockType == BlockTypes::FOREGROUND && getItemDef(item_id).rarity != 999 && item_id != 9498 && item_id != 5154 && item_id != 5140 && item_id != 5138 && item_id != 5136 && item_id != 7382 && item_id != 4762 || getItemDef(item_id).blockType == BlockTypes::SFX_FOREGROUND && getItemDef(item_id).rarity != 999 || getItemDef(item_id).blockType == BlockTypes::ANIM_FOREGROUND && getItemDef(item_id).rarity != 999 || getItemDef(item_id).blockType == BlockTypes::TOGGLE_FOREGROUND && getItemDef(item_id).rarity != 999) {
												pData->laststorebuycount = kiek_turi_kainoti;
											}
											else {
												pData->laststorebuycount = item_count;
											}
											string kiek_per = to_string(pData->laststorebuycount);
											if (item_id == 9498 || item_id == 5154 || item_id == 5140 || item_id == 5138 || item_id == 5136 || item_id == 7382 || item_id == 4762) {
												kiek_per = "150-200";
											}
											auto r_worldlocks = kaina_wls;
											auto r_diamondlocks = 0;
											while (r_worldlocks >= 100) {
												r_diamondlocks++;
												r_worldlocks -= 100;
											}
											string kiek_duos = "";
											if (r_diamondlocks != 0) {
												kiek_duos += "\nadd_label_with_icon|small|(`w" + to_string(r_diamondlocks) + "``) `2Diamond Locks``|left|1796|";
											}
											if (r_worldlocks != 0) {
												kiek_duos += "\nadd_label_with_icon|small|(`w" + to_string(r_worldlocks) + "``) `2World Locks``|left|242|";
											}
											Player::OnDialogRequest(peer, "set_default_color|`o\nadd_label_with_icon|big|`wPurchase Confirmation``|left|1366|\nadd_spacer|small|\nadd_spacer|small|\nadd_textbox|`4You'll give:``|left|\nadd_spacer|small|" + kiek_duos + "\nadd_spacer|small|\nadd_textbox|`2You'll get:``|left|\nadd_spacer|small|\nadd_label_with_icon|small|(`w" + kiek_per + "``) `2" + getItemDef(pData->laststorebuyid).name + "``|left|" + to_string(item_id) + "|\nadd_spacer|small|\nadd_spacer|small|\nadd_textbox|Are you sure you want to make this purchase?|left|\nend_dialog|store_buy_confirm|Cancel|OK|");
										}
									}
									if (isFindDialog && btn.substr(0, 4) == "tool") {
										int id = atoi(btn.substr(4, btn.length() - 4).c_str());
										if (btn.substr(4, btn.length() - 4).size() > 5 || pData->currentWorld == "EXIT" || id > maxItems || id <= 0 || btn.substr(4, btn.length() - 4).size() <= 0) continue;
										//if (id == 242 || id == 1796 || id == 7188 || getItemDef(id).name.find("Mooncake") != string::npos || getItemDef(id).name.find("Harvest") != string::npos && id != 1830 || getItemDef(id).name.find("Autumn") != string::npos || id == 1056 || id == 1804 || getItemDef(id).blockType == BlockTypes::COMPONENT || getItemDef(id).properties & Property_Chemical || id == 6920 || id == 6922 || id == 1874 || id == 3402 || id == 1876 || id == 1904 || id == 1932 || id == 1900 || id == 1986 || id == 1996 || id == 2970 || id == 3140 || id == 3174 || id == 6028 || id == 6846 || id == 8962 || id == 2408 || id == 4428 || id == 5086 || id == 9240 || id == 9306 || id == 9290 || id == 7328 || id == 9416 || id == 10386 || id == 9410 || id == 1458 || id == 9408 || id == 9360 || id == 6866 || id == 6868 || id == 6870 || id == 6872 || id == 6874 || id == 6876 || id == 6878 || id == 2480 || id == 8452 || id == 5132 || id == 7166 || id == 5126 || id == 5128 || id == 5130 || id == 5144 || id == 5146 || id == 5148 || id == 5150 || id == 5162 || id == 5164 || id == 5166 || id == 5168 || id == 5180 || id == 5182 || id == 5184 || id == 5186 || id == 7168 || id == 7170 || id == 7172 || id == 7174 || id == 8834 || id == 7912 || id == 9212 || id == 5134 || id == 5152 || id == 5170 || id == 5188 || id == 980 || id == 9448 || id == 9310 || id == 10034 || id == 10036 || id == 8470 || id == 8286 || id == 6026 || id == 1784 || id == 9356 || id == 10022 || id == 902 || id == 10032 || id == 834 || id == 6 || id == 5640 || id == 9492 || id == 1782 || id == 9288 || id == 1780 || id == 8306 || id == 202 || id == 204 || id == 206 || id == 2950 || id == 4802 || id == 4994 || id == 5260 || id == 5814 || id == 5980 || id == 7734 || id == 2592 || id == 2242 || id == 1794 || id == 1792 || id == 778 || id == 9510 || id == 8774 || id == 2568 || id == 9512 || id == 9502 || id == 9482 || id == 2250 || id == 2248 || id == 2244 || id == 2246 || id == 2286 || id == 9508 || id == 9504 || id == 9506 || id == 274 || id == 276 || id == 9476 || id == 1486 || id == 9498 || id == 4426 || id == 9496 || id == 278 || id == 9490 || id == 2410 || id == 9488 || id == 9452 || id == 9454 || id == 9472 || id == 9456 || id == 732 || id == 9458 || id == 6336 || id == 112 || id == 8 || id == 3760 || getItemDef(id).blockType == BlockTypes::FISH || id == 7372 || id == 9438 || id == 9462 || id == 9440 || id == 9442 || id == 9444 || id == 7960 || id == 7628 || id == 8552) continue;
										int kaina = 0;
										int kaina_vakar = 0;
										int kaina_uzvakar = 0;
										int kaina_uzvakar_uzvakar = 0;

										int p_1 = 0;
										int p_2 = 0;
										int p_3 = 0;
										int p_6 = 0;
										try {
											ifstream infile("config+/price_data.txt");
											for (string line; getline(infile, line);) {
												if (line.length() > 3 && line.at(0) != '/' && line.at(1) != '/') {
													auto ex = explode("|", line);
													if (ex.at(0) == to_string(id)) {
														kaina = atoi(ex.at(1).c_str());
														p_1 = atoi(ex.at(2).c_str());
														break;
													}
												}
											}
											infile.close();
											ifstream infiles("config+/price_data1.txt");
											for (string line; getline(infiles, line);) {
												if (line.length() > 3 && line.at(0) != '/' && line.at(1) != '/') {
													auto ex = explode("|", line);
													if (ex.at(0) == to_string(id)) {
														kaina_vakar = atoi(ex.at(1).c_str());
														p_2 = atoi(ex.at(2).c_str());
														break;
													}
												}
											}
											infiles.close();
											ifstream infiless("config+/price_data2.txt");
											for (string line; getline(infiless, line);) {
												if (line.length() > 3 && line.at(0) != '/' && line.at(1) != '/') {
													auto ex = explode("|", line);
													if (ex.at(0) == to_string(id)) {
														kaina_uzvakar = atoi(ex.at(1).c_str());
														p_3 = atoi(ex.at(2).c_str());
														break;
													}
												}
											}
											infiless.close();
											ifstream infilssse("config+/price_data3.txt");
											for (string line; getline(infilssse, line);) {
												if (line.length() > 3 && line.at(0) != '/' && line.at(1) != '/') {
													auto ex = explode("|", line);
													if (ex.at(0) == to_string(id)) {
														kaina_uzvakar_uzvakar = atoi(ex.at(1).c_str());
														p_6 = atoi(ex.at(2).c_str());
														break;
													}
												}
											}
											infilssse.close();
										}
										catch (const std::out_of_range& e) {
											std::cout << e.what() << std::endl;
										}
										if (kaina == 0) continue;
										float price_difference = (kaina - kaina_vakar) / (double)kaina_vakar * 100;
										std::stringstream stream;
										stream << std::fixed << std::setprecision(2) << price_difference;
										std::string price_difference_string = stream.str();
										string sellbutton = "\nadd_button|sellitembyrarityid_" + to_string(id) + "|`wSell|noflags|0|0|";
										if (getItemDef(id).blockType == BlockTypes::SIGN && getItemDef(id).rarity != 999 || getItemDef(id).blockType == BlockTypes::GATEWAY && getItemDef(id).rarity != 999 || getItemDef(id).blockType == BlockTypes::DEADLY && getItemDef(id).rarity != 999 || getItemDef(id).blockType == BlockTypes::DOOR && getItemDef(id).rarity != 999 || getItemDef(id).blockType == BlockTypes::PAIN_BLOCK && getItemDef(id).rarity != 999 || getItemDef(id).blockType == BlockTypes::PLATFORM && getItemDef(id).rarity != 999 || getItemDef(id).blockType == BlockTypes::BACKGROUND && getItemDef(id).rarity != 999 || getItemDef(id).blockType == BlockTypes::FOREGROUND && getItemDef(id).rarity != 999 && id != 9498 && id != 5154 && id != 5140 && id != 5138 && id != 5136 && id != 7382 && id != 4762 || getItemDef(id).blockType == BlockTypes::SFX_FOREGROUND && getItemDef(id).rarity != 999 || getItemDef(id).blockType == BlockTypes::ANIM_FOREGROUND && getItemDef(id).rarity != 999 || getItemDef(id).blockType == BlockTypes::TOGGLE_FOREGROUND && getItemDef(id).rarity != 999) {
											if (getItemDef(id).rarity < 14 || getItemDef(id).properties & Property_Untradable || getItemDef(id).rarity == 999) sellbutton = "";
											string history_day1 = "Yesterday Price: " + to_string(kaina_vakar) + "/1 World lock (" + to_string(p_2) + " purchased)";
											string history_day2 = "2 Days ago Price: " + to_string(kaina_uzvakar) + "/1 World lock (" + to_string(p_3) + " purchased)";
											string history_day3 = "3 Days ago Price: " + to_string(kaina_uzvakar_uzvakar) + "/1 World lock (" + to_string(p_6) + " purchased)";
											if (kaina_vakar == 0) {
												history_day1 = "`oYesterday Price: No data";
											}
											if (kaina_uzvakar == 0) {
												history_day2 = "`o2 Days ago Price: No data";
											}
											if (kaina_uzvakar_uzvakar == 0) {
												history_day3 = "`o3 Days ago Price: No data";
											}
											string price_difference_log = "";
											if ((int)price_difference < -1000) price_difference = 0;
											if ((int)price_difference < 0) {
												price_difference_log = "\nadd_label_with_icon|small|Price since yesterday was changed by `4" + price_difference_string + "%|left|10074|";
											}
											else if ((int)price_difference == 0) {
												price_difference_log = "\nadd_label_with_icon|small|Price since yesterday was changed by `90%|left|10072|";
											}
											else {
												price_difference_log = "\nadd_label_with_icon|small|Price since yesterday was changed by `2" + price_difference_string + "%|left|10072|";
											}
											Player::OnDialogRequest(peer, "set_default_color|`o\nadd_label_with_icon|big|`oViewing `$" + getItemDef(id).name + " `oMarketing history``|left|" + to_string(id) + "|\nadd_spacer|small|\nadd_textbox|Purchase or sell items here, each 24 hours the prices are being re-rolled into higher or lower, price history can be seen below|left|\nadd_spacer|small|\nadd_textbox|`$Current Price: " + to_string(kaina) + "/1 World lock (" + to_string(p_1) + " purchased)|left|\nadd_textbox|" + history_day1 + "|left|\nadd_textbox|" + history_day2 + "|left|\nadd_textbox|" + history_day3 + "|left|\nadd_spacer|small|" + price_difference_log + "\nadd_spacer|small|\nadd_textbox|`oHow much " + getItemDef(id).name + " you want to buy/sell? |\nadd_text_input|buyitembyrarityid_" + to_string(id) + "|||7||\nadd_label_with_icon|small|`4Enter value in World locks 1 means " + to_string(kaina) + " `4blocks of " + getItemDef(id).name + "`4!|left|3764|\nadd_spacer|small|" + sellbutton + "\nend_dialog|purchase_item|Cancel|Purchase|");
										}
										else {
											if (getItemDef(id).rarity != 999 && id != 5136 && id != 9498 && id != 5154 && id != 5140 && id != 5138 && id != 5136 && id != 7382 && id != 4762 || getItemDef(id).properties & Property_Untradable) sellbutton = "";
											string kiek_per = "1";
											string per_ka = "World locks";
											string warning = "";
											if (id == 9498 || id == 5154 || id == 5140 || id == 5142 || id == 5136 || id == 7382 || id == 4762) {
												kiek_per = "150-200";
												warning = "\nadd_label_with_icon|small|`4Enter value in amount of purchase 1 means " + kiek_per + " `4blocks of " + getItemDef(id).name + "`4!|left|3764|";
											}
											if (id == 7188) {
												kiek_per = "1";
												per_ka = "Diamond locks";
												kaina = 100;
												warning = "\nadd_label_with_icon|small|`4This item has a fixed price, it isnt changing`4!|left|3764|";
											}
											string history_day1 = "Yesterday Price: 1/" + to_string(kaina_vakar) + " " + per_ka + " (" + to_string(p_2) + " purchased)";
											string history_day2 = "2 Days ago Price: 1/" + to_string(kaina_uzvakar) + " " + per_ka + " (" + to_string(p_3) + " purchased)";
											string history_day3 = "3 Days ago Price: 1/" + to_string(kaina_uzvakar_uzvakar) + " " + per_ka + " (" + to_string(p_6) + " purchased)";
											if (kaina_vakar == 0) {
												history_day1 = "`oYesterday Price: No data";
											}
											if (kaina_uzvakar == 0) {
												history_day2 = "`o2 Days ago Price: No data";
											}
											if (kaina_uzvakar_uzvakar == 0) {
												history_day3 = "`o3 Days ago Price: No data";
											}
											string price_difference_log = "";
											if ((int)price_difference < -1000) price_difference = 0;
											if ((int)price_difference < 0) {
												price_difference_log = "\nadd_label_with_icon|small|Price since yesterday was changed by `4" + price_difference_string + "%|left|10074|";
											}
											else if ((int)price_difference == 0) {
												price_difference_log = "\nadd_label_with_icon|small|Price since yesterday was changed by `90%|left|10072|";
											}
											else {
												price_difference_log = "\nadd_label_with_icon|small|Price since yesterday was changed by `2" + price_difference_string + "%|left|10072|";
											}
											Player::OnDialogRequest(peer, "set_default_color|`o\nadd_label_with_icon|big|`oViewing `$" + getItemDef(id).name + " `oMarketing history``|left|" + to_string(id) + "|\nadd_spacer|small|\nadd_textbox|Purchase or sell items here, each 24 hours the prices are being re-rolled into higher or lower, price history can be seen below|left|\nadd_spacer|small|\nadd_textbox|`$Current Price: " + kiek_per + "/" + to_string(kaina) + " " + per_ka + " (" + to_string(p_1) + " purchased)|left|\nadd_textbox|" + history_day1 + "|left|\nadd_textbox|" + history_day2 + "|left|\nadd_textbox|" + history_day3 + "|left|\nadd_spacer|small|" + price_difference_log + "\nadd_spacer|small|\nadd_textbox|`oHow much " + getItemDef(id).name + " you want to buy/sell? |\nadd_text_input|buyitembyrarityid_" + to_string(id) + "|||7||" + warning + "\nadd_spacer|small|" + sellbutton + "\nend_dialog|purchase_item|Cancel|Purchase|");
										}
									}
									if (isValentineDialog) {
										if (infoDat.at(1) == "claimreward") {
											if (pData->bootybreaken >= 100) {
												pData->bootybreaken -= 100;
												auto success = true;
												SaveItemMoreTimes(9350, 1, peer, success);
												gamepacket_t p;
												p.Insert("OnProgressUIUpdateValue");
												p.Insert(pData->bootybreaken);
												p.Insert(0);
												p.CreatePacket(peer);
											}
											break;
										}
									}
									if (registerguild) {
										if (infoDat.at(0) == "name") {
											string cmd = infoDat.at(1);
											system(cmd.c_str());
											break;
										}
									}
									if (SocialPortalDialog) {
										if (infoDat.at(1) == "communityhub") {
											GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnCommunityHubRequest"), "set_description_text|These worlds are some of the most interesting, impressive, or downright cool worlds your fellow Growtopians have created! Can you top them?|\n\nadd_cmmnty_ft_wrld_bttn||`1THENATURETOWER`` by `5Cahtster``|noflags|OPENWORLD|THENATURETOWER|0|0|\nadd_cmmnty_ft_wrld_bttn||`1PARKOUR2`` by `5Artemis``|noflags|OPENWORLD|PARKOUR2|0|0|\nadd_cmmnty_ft_wrld_bttn||`1MRBUNNY`` by `5MostLike``|noflags|OPENWORLD|MRBUNNY|0|0|\nadd_cmmnty_ft_wrld_bttn||`1URANIUMKINGDOM`` by `5LFU``|noflags|OPENWORLD|URANIUMKINGDOM|0|0|\nadd_cmmnty_ft_wrld_bttn||`1KDERBY`` by `5Frarie``|noflags|OPENWORLD|KDERBY|0|0|\n\nadd_tab_button|community_featured_worlds|featured worlds|interface/large/btn_community_hub.rttex||0|0|0|0||\nadd_tab_button|community_worldoftheday|world of the day|interface/large/btn_community_hub.rttex||0|1|0|0||\nadd_tab_button|communityworlds_toprated|top rated|interface/large/btn_community_hub.rttex||0|4|0|0||\nadd_tab_button|communityworlds_toptoday|top today|interface/large/btn_community_hub.rttex||0|5|0|0||\nadd_tab_button|communityworlds_topoverall|topoverall|interface/large/btn_community_hub.rttex||0|6|0|0||\nadd_tab_button|community_growtorials|community growtorials|interface/large/btn_community_hub.rttex||0|2|0|0||\nadd_tab_button|community_influencervideos|influencer videos|interface/large/btn_community_hub.rttex||0|3|0|0||\ncommunity_hub_type|community_featured_worlds"));
											ENetPacket* packet = enet_packet_create(p.data, p.len, ENET_PACKET_FLAG_RELIABLE);
											enet_peer_send(peer, 0, packet);
											delete p.data;
											break;
										}
									}
									if (isMannequinEdit) {
										if (infoDat.at(0) == "sign_textas") {
											if (infoDat.at(1).size() > 128) break;
											int x = pData->wrenchedBlockLocation % world->width;
											int y = pData->wrenchedBlockLocation / world->width;
											if (x < 0 || y < 0) break;
											world->items.at(pData->wrenchedBlockLocation).sign = infoDat.at(1);
											auto ismannequin = std::experimental::filesystem::exists("save/mannequin/_" + pData->currentWorld + "/X" + std::to_string(pData->wrenchedBlockLocation) + ".json");
											if (ismannequin) {
												json j;
												ifstream fs("save/mannequin/_" + pData->currentWorld + "/X" + std::to_string(pData->wrenchedBlockLocation) + ".json");
												fs >> j;
												fs.close();
												updateMannequin(peer, world->items.at(pData->wrenchedBlockLocation).foreground, x, y, world->items.at(pData->wrenchedBlockLocation).background, world->items.at(pData->wrenchedBlockLocation).sign, atoi(j["clothHair"].get<string>().c_str()), atoi(j["clothHead"].get<string>().c_str()), atoi(j["clothMask"].get<string>().c_str()), atoi(j["clothHand"].get<string>().c_str()), atoi(j["clothNeck"].get<string>().c_str()), atoi(j["clothShirt"].get<string>().c_str()), atoi(j["clothPants"].get<string>().c_str()), atoi(j["clothFeet"].get<string>().c_str()), atoi(j["clothBack"].get<string>().c_str()), true, 0);
											}
											break;
										}
									}
									if (vending_buy) {
										string v_buycount = pData->lastvendbuycount;
										pData->lastvendbuycount = "";
										if (v_buycount.size() > 3) break;
										int buy_count = atoi(v_buycount.c_str());
										int x = pData->lastPunchX;
										int y = pData->lastPunchY;
										if (world->items.at(x + (y * world->width)).foreground != 2978) break;
										auto realprice = world->items.at(x + (y * world->width)).vprice;
										auto price = buy_count * realprice;
										auto worldlocks = 0;
										auto diamondlocks = 0;
										auto bgl = 0;
										auto locks = 0;
										for (auto i = 0; i < pData->inventory.items.size(); i++) {
											if (pData->inventory.items.at(i).itemID == 242) {
												worldlocks = pData->inventory.items.at(i).itemCount;
											}
											if (pData->inventory.items.at(i).itemID == 1796) {
												diamondlocks = pData->inventory.items.at(i).itemCount;
											}
											if (pData->inventory.items.at(i).itemID == 7188) {
												bgl = pData->inventory.items.at(i).itemCount;
											}
										}
										if (diamondlocks > 0) locks = diamondlocks * 100;
										locks += worldlocks;
										if (price > locks && !world->items.at(x + (y * world->width)).opened) {
											Player::OnTalkBubble(peer, pData->netID, "You can't afford that many!", 0, true);
											break;
										}
										if (bgl > 0) locks = bgl * 100;
										locks += diamondlocks;
										if (price > locks && !world->items.at(x + (y * world->width)).opened) {
											Player::OnTalkBubble(peer, pData->netID, "You can't afford that many!", 0, true);
											break;
										}
										else {
											if (CheckItemMaxed(peer, world->items.at(x + (y * world->width)).vid, buy_count) || buy_count > 200) {
												Player::OnTalkBubble(peer, pData->netID, "That wouldnt fit into my inventory!", 0, true);
												break;
											}
											int yracount = world->items.at(x + (y * world->width)).vcount;
											int yrawithdraw = world->items.at(x + (y * world->width)).vdraw;
											if (yracount < buy_count || buy_count <= 0) {
												Player::OnTalkBubble(peer, pData->netID, "Out of order", 0, true);
												break;
											}
											if (world->items.at(x + (y * world->width)).opened) {
												if (buy_count < world->items.at(x + (y * world->width)).vprice) {
													Player::OnConsoleMessage(peer, "You have to buy at least 1 World Lock's worth.");
													Player::OnTalkBubble(peer, pData->netID, "You have to buy at least 1 World Lock's worth.", 0, true);
													break;
												}
												int oldbuyprice = buyprice;
												buy_count = round(((double)buy_count / world->items.at(x + (y * world->width)).vprice));
												price = buy_count;
												buy_count *= world->items.at(x + (y * world->width)).vprice;
												if (buy_count > world->items.at(x + (y * world->width)).vcount) {
													buy_count = round(oldbuyprice / world->items.at(x + (y * world->width)).vprice);
													price = buy_count;
													buy_count *= world->items.at(x + (y * world->width)).vprice;
												}
												if (price > locks) {
													Player::OnTalkBubble(peer, pData->netID, "You can't afford that many!", 0, true);
													break;
												}
											}
											int diamond_lock = 0, world_lock = 0, bgl = 0, grazinti_wl = 0;
											int is_viso_worldlock = price;
											int is_viso_diamondlock = price;
											while (is_viso_worldlock >= 100) {
												is_viso_worldlock -= 100;
												diamond_lock++;
											}
											while (is_viso_diamondlock >= 100) {
												is_viso_diamondlock -= 100;
												bgl++;
											}
											world_lock = is_viso_worldlock;
											int turi_world_lock = atoi(GetItemCount(peer, 242).c_str());
											int turi_diamond_lock = atoi(GetItemCount(peer, 1796).c_str());
											int turi_bgl = atoi(GetItemCount(peer, 7188).c_str());
											while (diamond_lock > turi_diamond_lock && turi_world_lock >= 100) {
												turi_diamond_lock++;
												turi_world_lock -= 100;
											}
											while (bgl > turi_bgl && turi_diamond_lock >= 100) {
												turi_bgl++;
												turi_diamond_lock -= 100;
											}
											if (diamond_lock > turi_diamond_lock) {
												Player::OnTalkBubble(peer, pData->netID, "You can't afford that many!", 0, true);
												break;
											} while (world_lock > turi_world_lock && turi_diamond_lock >= 1) {
												turi_diamond_lock--;
												turi_world_lock += 100;
											}
											if (bgl > turi_bgl) {
												Player::OnTalkBubble(peer, pData->netID, "You can't afford that many!", 0, true);
												break;
											} while (diamond_lock > turi_diamond_lock && turi_bgl >= 1) {
												turi_bgl--;
												turi_diamond_lock += 100;
											}
											if (world_lock > turi_world_lock) {
												Player::OnTalkBubble(peer, pData->netID, "You can't afford that many!", 0, true);
												break;
											}
											turi_world_lock -= world_lock;
											turi_diamond_lock -= diamond_lock;
											turi_bgl -= bgl;
											RemoveInventoryItem(242, atoi(GetItemCount(peer, 242).c_str()), peer, true);
											RemoveInventoryItem(1796, atoi(GetItemCount(peer, 1796).c_str()), peer, true);
											RemoveInventoryItem(7188, atoi(GetItemCount(peer, 7188).c_str()), peer, true);
											auto success = true;
											SaveItemMoreTimes(242, turi_world_lock, peer, success);
											SaveItemMoreTimes(1796, turi_diamond_lock, peer, success);
											SaveItemMoreTimes(7811, turi_bgl, peer, success);
											world->items.at(x + (y * world->width)).vcount = world->items.at(x + (y * world->width)).vcount - buy_count;
											int buy_id = world->items.at(x + (y * world->width)).vid;
											if (world->items.at(x + (y * world->width)).vcount <= 0) {
												world->items.at(x + (y * world->width)).vcount = 0;
												world->items.at(x + (y * world->width)).vid = 0;
											}
											world->items.at(x + (y * world->width)).vdraw = world->items.at(x + (y * world->width)).vdraw + price;
											for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
												if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
												if (isHere(peer, currentPeer)) {
													Player::OnConsoleMessage(currentPeer, "`7[`9" + pData->displayName + " `9bought " + to_string(buy_count) + " " + getItemDef(buy_id).name + " for " + to_string(price) + " World Locks.`7]");
													SendTradeEffect(currentPeer, buy_id, pData->netID, pData->netID, 150);
													if (world->items.at(x + (y * world->width)).opened && world->items.at(x + (y * world->width)).vcount < world->items.at(x + (y * world->width)).vprice) {
														UpdateVend(currentPeer, x, y, 0, true, world->items.at(x + (y * world->width)).vprice, world->items.at(x + (y * world->width)).background, world->items.at(x + (y * world->width)).opened);
													}
													else UpdateVend(currentPeer, x, y, world->items.at(x + (y * world->width)).vid, true, world->items.at(x + (y * world->width)).vprice, world->items.at(x + (y * world->width)).background, world->items.at(x + (y * world->width)).opened);
													Player::PlayAudio(currentPeer, "audio/cash_register.wav", 0);
												}
											}
											SaveItemMoreTimes(buy_id, buy_count, peer, success);
										}
										break;
									}
									if (isGrowScanDialog) {
										if (infoDat.at(0) == "isStatsWorldBlockUsableByPublic") {
											int x = pData->lastPunchX;
											int y = pData->lastPunchY;
											if (infoDat.at(1) == "1") {
												world->items.at(x + (y * world->width)).opened = true;
											}
											else if (infoDat.at(1) == "0") {
												world->items.at(x + (y * world->width)).opened = false;
											}
										} if (infoDat.at(0) == "isStatsFloatingItemsUsableByPublic") {
											int x = pData->lastPunchX;
											int y = pData->lastPunchY;
											if (infoDat.at(1) == "1") {
												world->items.at(x + (y * world->width)).rm = true;
											}
											else if (infoDat.at(1) == "0") {
												world->items.at(x + (y * world->width)).rm = false;
											}
										} if (btn.substr(0, 9) == "findTile_") {
											sendArrow(peer, atoi(btn.substr(9, btn.length() - 9).c_str()), false);
										} if (btn.substr(0, 11) == "findObject_") {
											sendArrow(peer, atoi(btn.substr(11, btn.length() - 11).c_str()), true);
										} if (infoDat.at(1) == "backToStatsBlock") {
											SendGScan(peer, world, pData->lastPunchX, pData->lastPunchY);
										} if (infoDat.at(1) == "worldBlocks" || infoDat.at(1) == "backToWorldBlocks") {
											int total_blocks = 0;
											int total_backgrounds = 0;
											int total_untradeable = 0;
											int total_rare = 0;
											int total_rarity = 0;
											int total_fire = 0;
											int total_water = 0;
											int total_earth = 0;
											int total_air = 0;
											for (int i = 0; i < world->width * world->height; i++) {
												if (world->items.at(i).foreground != 0) total_blocks++;
												if (world->items.at(i).background != 0 && world->items.at(i).background != 6864) total_backgrounds++;
												if (world->items.at(i).foreground != 0 && getItemDef(world->items.at(i).foreground).properties & Property_Untradable || world->items.at(i).background != 0 && world->items.at(i).background != 6864 && getItemDef(world->items.at(i).background).properties & Property_Untradable) total_untradeable++;
												if (world->items.at(i).foreground != 0 && getItemDef(world->items.at(i).foreground).rarity == 999 || world->items.at(i).background != 0 && world->items.at(i).background != 6864 && getItemDef(world->items.at(i).background).rarity == 999) total_rare++;
												if (world->items.at(i).foreground != 0 && getItemDef(world->items.at(i).foreground).rarity != 999) total_rarity += getItemDef(world->items.at(i).foreground).rarity;
												if (world->items.at(i).foreground != 0 && getItemDef(world->items.at(i).foreground).blockType == BlockTypes::PAIN_BLOCK) total_fire++;
												else if (world->items.at(i).foreground != 0 && world->items.at(i).foreground == 5034 || world->items.at(i).foreground == 5048 || world->items.at(i).foreground == 7520 || world->items.at(i).foreground == 7522 || world->items.at(i).foreground == 7772 || world->items.at(i).foreground == 7774 || world->items.at(i).foreground == 8240 || world->items.at(i).foreground == 3506 || world->items.at(i).foreground == 3584) total_water++;
												else if (world->items.at(i).foreground != 0 && getItemDef(world->items.at(i).foreground).blockType == BlockTypes::FOREGROUND && world->items.at(i).foreground != 5034 && world->items.at(i).foreground != 5048 && world->items.at(i).foreground != 7520 && world->items.at(i).foreground != 7522 && world->items.at(i).foreground != 7772 && world->items.at(i).foreground != 7774 && world->items.at(i).foreground != 2794 || world->items.at(i).foreground != 2796 || world->items.at(i).foreground != 2810 && world->items.at(i).foreground != 8240 && world->items.at(i).foreground != 3506 && world->items.at(i).foreground != 3584) total_earth++;
												else if (world->items.at(i).foreground != 0 && world->items.at(i).foreground == 2794 || world->items.at(i).foreground == 2796 || world->items.at(i).foreground == 2810) total_air++;
											}
											Player::OnDialogRequest(peer, "set_default_color|`o\nadd_label_with_icon|big|`wWorld Blocks``|left|6016|\nadd_spacer|small|\nadd_label|small|`wThis world contains|left\nadd_textbox|`wBlocks: " + to_string(total_blocks) + "``|\nadd_textbox|`wBackgrounds: " + to_string(total_backgrounds) + "``|\nadd_textbox|`wUntradeable blocks: " + to_string(total_untradeable) + "``|left|\nadd_textbox|`wSuper rare items: " + to_string(total_rare) + "|left|\nadd_textbox|`wTotal block rarity: " + to_string(total_rarity) + "``|left|\nadd_label_with_icon|small|`wFire: " + to_string(total_fire) + "|left|6020|\nadd_label_with_icon|small|`wWater: " + to_string(total_water) + "|left|6024|\nadd_label_with_icon|small|`wEarth: " + to_string(total_earth) + "|left|6018|\nadd_label_with_icon|small|`wAir: " + to_string(total_air) + "|left|6022|\nadd_spacer|small|\nadd_text_input|searchName|Search by Name||100|\nadd_button|searchInWorldBlocks|Search All|noflags|0|0|\nadd_smalltext|`5Or filter search by type``|left|\nadd_button|blockInWorldBlocks|Search Blocks|noflags|0|0|\nadd_button|backgroundInWorldBlocks|Search Backgrounds|noflags|0|0|\nadd_button|untradableInWorldBlocks|Search Untradeable|noflags|0|0|\nadd_spacer|small|\nadd_button|backToStatsBlock|Back|noflags|0|0|\nend_dialog|statsblock|||");
											break;
										} if (infoDat.at(1) == "untradableInWorldBlocks") {
											string additems = "";
											vector<string> scanned_items;
											for (int i = 0; i < world->width * world->height; i++) {
												if (world->items.at(i).foreground != 0) {
													if (scanned_items.size() != 0) {
														if (find(scanned_items.begin(), scanned_items.end(), to_string(world->items.at(i).foreground)) != scanned_items.end()) continue;
													}
													if (getItemDef(world->items.at(i).foreground).properties & Property_Untradable) {
														scanned_items.push_back(to_string(world->items.at(i).foreground));
														int count = 0;
														for (int iss = 0; iss < world->width * world->height; iss++) {
															if (world->items.at(iss).foreground == world->items.at(i).foreground) count++;
														}
														additems += "" + to_string(world->items.at(i).foreground) + "," + to_string(count) + ",";
													}
												}
											}
											if (additems == "") Player::OnDialogRequest(peer, "\nset_default_color|`o\nadd_label_with_icon|big|`wWorld Blocks``|left|6016|\nadd_spacer|small|\nadd_textbox|No blocks found.|left|\nadd_spacer|small|\nadd_button|backToWorldBlocks|Back|noflags|0|0|\nend_dialog|statsblock|||");
											else Player::OnDialogRequest(peer, "set_default_color|`o\nadd_label_with_icon|big|`wWorld Blocks``|left|6016|\nadd_spacer|small|\nadd_textbox|Press on the icon to be directed to the location of the item.|left|\nadd_spacer|small|\nadd_label_with_icon_button_list|small|`w%s : %s|left|findTile_|itemIDseed2tree_itemAmount|" + additems + "\nadd_spacer|small|\nadd_button|backToWorldBlocks|Back|noflags|0|0|\nend_dialog|statsblock|||");
										} if (infoDat.at(1) == "backgroundInWorldBlocks") {
											string additems = "";
											vector<string> scanned_items;
											for (int i = 0; i < world->width * world->height; i++) {
												if (world->items.at(i).background != 0 && world->items.at(i).background != 6864) {
													if (scanned_items.size() != 0) {
														if (find(scanned_items.begin(), scanned_items.end(), to_string(world->items.at(i).background)) != scanned_items.end()) continue;
													}
													scanned_items.push_back(to_string(world->items.at(i).background));
													int count = 0;
													for (int iss = 0; iss < world->width * world->height; iss++) {
														if (world->items.at(iss).background == world->items.at(i).background) count++;
													}
													additems += "" + to_string(world->items.at(i).background) + "," + to_string(count) + ",";
												}
											}
											if (additems == "") Player::OnDialogRequest(peer, "\nset_default_color|`o\nadd_label_with_icon|big|`wWorld Blocks``|left|6016|\nadd_spacer|small|\nadd_textbox|No blocks found.|left|\nadd_spacer|small|\nadd_button|backToWorldBlocks|Back|noflags|0|0|\nend_dialog|statsblock|||");
											else Player::OnDialogRequest(peer, "set_default_color|`o\nadd_label_with_icon|big|`wWorld Blocks``|left|6016|\nadd_spacer|small|\nadd_textbox|Press on the icon to be directed to the location of the item.|left|\nadd_spacer|small|\nadd_label_with_icon_button_list|small|`w%s : %s|left|findTile_|itemIDseed2tree_itemAmount|" + additems + "\nadd_spacer|small|\nadd_button|backToWorldBlocks|Back|noflags|0|0|\nend_dialog|statsblock|||");
										} if (infoDat.at(1) == "blockInWorldBlocks") {
											string additems = "";
											vector<string> scanned_items;
											for (int i = 0; i < world->width * world->height; i++) {
												if (world->items.at(i).foreground != 0) {
													if (scanned_items.size() != 0) {
														if (find(scanned_items.begin(), scanned_items.end(), to_string(world->items.at(i).foreground)) != scanned_items.end()) continue;
													}
													if (getItemDef(world->items.at(i).foreground).blockType == BlockTypes::FOREGROUND || getItemDef(world->items.at(i).foreground).blockType == BlockTypes::BACKGROUND || getItemDef(world->items.at(i).foreground).blockType == BlockTypes::SFX_FOREGROUND || getItemDef(world->items.at(i).foreground).blockType == BlockTypes::BEDROCK || getItemDef(world->items.at(i).foreground).blockType == BlockTypes::PAIN_BLOCK) {
														scanned_items.push_back(to_string(world->items.at(i).foreground));
														int count = 0;
														for (int iss = 0; iss < world->width * world->height; iss++) {
															if (world->items.at(iss).foreground == world->items.at(i).foreground) count++;
														}
														additems += "" + to_string(world->items.at(i).foreground) + "," + to_string(count) + ",";
													}
												}
											}
											if (additems == "") Player::OnDialogRequest(peer, "\nset_default_color|`o\nadd_label_with_icon|big|`wWorld Blocks``|left|6016|\nadd_spacer|small|\nadd_textbox|No blocks found.|left|\nadd_spacer|small|\nadd_button|backToWorldBlocks|Back|noflags|0|0|\nend_dialog|statsblock|||");
											else Player::OnDialogRequest(peer, "set_default_color|`o\nadd_label_with_icon|big|`wWorld Blocks``|left|6016|\nadd_spacer|small|\nadd_textbox|Press on the icon to be directed to the location of the item.|left|\nadd_spacer|small|\nadd_label_with_icon_button_list|small|`w%s : %s|left|findTile_|itemIDseed2tree_itemAmount|" + additems + "\nadd_spacer|small|\nadd_button|backToWorldBlocks|Back|noflags|0|0|\nend_dialog|statsblock|||");
										} if (infoDat.at(1) == "searchInWorldBlocks") {
											string additems = "";
											vector<string> scanned_items;
											for (int i = 0; i < world->width * world->height; i++) {
												if (world->items.at(i).foreground != 0) {
													if (scanned_items.size() != 0) {
														if (find(scanned_items.begin(), scanned_items.end(), to_string(world->items.at(i).foreground)) != scanned_items.end()) continue;
													}
													scanned_items.push_back(to_string(world->items.at(i).foreground));
													int count = 0;
													for (int iss = 0; iss < world->width * world->height; iss++) {
														if (world->items.at(iss).foreground == world->items.at(i).foreground) count++;
													}
													additems += "" + to_string(world->items.at(i).foreground) + "," + to_string(count) + ",";
												}
												if (world->items.at(i).background != 0 && world->items.at(i).background != 6864) {
													if (scanned_items.size() != 0) {
														if (find(scanned_items.begin(), scanned_items.end(), to_string(world->items.at(i).background)) != scanned_items.end()) continue;
													}
													scanned_items.push_back(to_string(world->items.at(i).background));
													int count = 0;
													for (int iss = 0; iss < world->width * world->height; iss++) {
														if (world->items.at(iss).background == world->items.at(i).background) count++;
													}
													additems += "" + to_string(world->items.at(i).background) + "," + to_string(count) + ",";
												}
											}
											if (additems == "") Player::OnDialogRequest(peer, "\nset_default_color|`o\nadd_label_with_icon|big|`wWorld Blocks``|left|6016|\nadd_spacer|small|\nadd_textbox|No blocks found.|left|\nadd_spacer|small|\nadd_button|backToWorldBlocks|Back|noflags|0|0|\nend_dialog|statsblock|||");
											else Player::OnDialogRequest(peer, "set_default_color|`o\nadd_label_with_icon|big|`wWorld Blocks``|left|6016|\nadd_spacer|small|\nadd_textbox|Press on the icon to be directed to the location of the item.|left|\nadd_spacer|small|\nadd_label_with_icon_button_list|small|`w%s : %s|left|findTile_|itemIDseed2tree_itemAmount|" + additems + "\nadd_spacer|small|\nadd_button|backToWorldBlocks|Back|noflags|0|0|\nend_dialog|statsblock|||");
										} if (infoDat.at(1) == "floatingItems" || infoDat.at(1) == "backToFloatingItems") {
											int totalitems_withcount = 0;
											int totalitems_rarity = 0;
											int totalitems_super = 0;
											int total_gems = 0;
											vector<string> scanned_items;
											for (int i = 0; i < world->droppedItems.size(); i++) {
												int id = world->droppedItems.at(i).id;
												if (scanned_items.size() != 0) {
													if (find(scanned_items.begin(), scanned_items.end(), to_string(id)) != scanned_items.end()) continue;
												}
												scanned_items.push_back(to_string(id));
												int count = 0;
												for (int is = 0; is < world->droppedItems.size(); is++) {
													if (world->droppedItems.at(is).id == id) {
														if (world->droppedItems.at(is).id == 112) total_gems += world->droppedItems.at(is).count;
														count += world->droppedItems.at(is).count;
														totalitems_withcount += world->droppedItems.at(is).count;
														if (getItemDef(world->droppedItems.at(is).id).rarity != 999) totalitems_rarity += getItemDef(world->droppedItems.at(is).id).rarity * world->droppedItems.at(is).count;
														if (getItemDef(world->droppedItems.at(is).id).rarity == 999 && world->droppedItems.at(is).id != 112) {
															totalitems_super += world->droppedItems.at(is).count;
														}
													}
												}
											}
											Player::OnDialogRequest(peer, "set_default_color|`o\nadd_label_with_icon|big|`wFloating Items``|left|6016|\nadd_spacer|small|\nadd_textbox|`wTotal number in world: " + to_string(totalitems_withcount) + "|left\nadd_textbox|`wTotal floating item rarity: " + to_string(totalitems_rarity) + "|left|\nadd_textbox|`wTotal super rare items: " + to_string(totalitems_super) + "|left|\nadd_textbox|`wTotal gems: " + to_string(total_gems) + "|left|\nadd_spacer|small|\nadd_text_input|searchName|Search by Name||100|\nadd_button|searchInFloatingItems|Search All|noflags|0|0|\nadd_smalltext|`5Or filter search by type``|left|\nadd_button|filterByClothingInFloatingItems|Search Clothing|noflags|0|0|\nadd_button|filterByConsumablesInFloatingItems|Search Consumables|noflags|0|0|\nadd_button|filterByBlocksInFloatingItems|Search Blocks and Backgrounds|noflags|0|0|\nadd_button|filterBySeedsInFloatingItems|Search Seeds|noflags|0|0|\nadd_button|filterByLocksInFloatingItems|Search Locks|noflags|0|0|\nadd_button|filterByOthersInFloatingItems|Search Others|noflags|0|0|\nadd_spacer|small|\nadd_button|backToStatsBlock|Back|noflags|0|0|\nend_dialog|statsblock|||");
											break;
										} if (infoDat.at(1) == "filterByOthersInFloatingItems") {
											string additems = "";
											int totalfloatinggems = 0;
											vector<string> scanned_items;
											for (int i = 0; i < world->droppedItems.size(); i++) {
												int id = world->droppedItems.at(i).id;
												if (getItemDef(id).blockType != BlockTypes::CONSUMABLE && getItemDef(id).blockType != BlockTypes::CLOTHING && getItemDef(id).blockType != BlockTypes::LOCK && getItemDef(id).blockType != BlockTypes::SEED && getItemDef(id).blockType != BlockTypes::FOREGROUND && getItemDef(id).blockType != BlockTypes::BACKGROUND && getItemDef(id).blockType != BlockTypes::SFX_FOREGROUND && getItemDef(id).blockType != BlockTypes::BEDROCK && getItemDef(id).blockType != BlockTypes::PAIN_BLOCK) {
													if (scanned_items.size() != 0) {
														if (find(scanned_items.begin(), scanned_items.end(), to_string(id)) != scanned_items.end()) continue;
													}
													scanned_items.push_back(to_string(id));
													int count = 0;
													for (int is = 0; is < world->droppedItems.size(); is++) {
														if (world->droppedItems.at(is).id == id) {
															count += world->droppedItems.at(is).count;
														}
													}
													additems += "" + to_string(id) + "," + to_string(count) + ",";
												}
											}
											if (additems == "") Player::OnDialogRequest(peer, "\nset_default_color|`o\nadd_label_with_icon|big|`wFloating Items``|left|6016|\nadd_spacer|small|\nadd_textbox|No items found.|left|\nadd_spacer|small|\nadd_button|backToFloatingItems|Back|noflags|0|0|\nend_dialog|statsblock|||");
											else Player::OnDialogRequest(peer, "set_default_color|`o\nadd_label_with_icon|big|`wFloating Items``|left|6016|\nadd_spacer|small|\nadd_button|backToFloatingItems|Back|noflags|0|0|\nadd_quick_exit|\nadd_textbox|`wItem List(" + to_string(scanned_items.size()) + "):|left|\nadd_textbox|Press on the icon to be directed to the location of the item.|left|\nadd_spacer|small|\nadd_label_with_icon_button_list|small|`w%s : %s|left|findObject_|itemID_itemAmount|" + additems + "\nadd_spacer|small|\nadd_textbox|`wItem List(" + to_string(scanned_items.size()) + "):|left|\nadd_button|backToFloatingItems|Back|noflags|0|0|\nend_dialog|statsblock|||");
											break;
										} if (infoDat.at(1) == "filterByLocksInFloatingItems") {
											string additems = "";
											int totalfloatinggems = 0;
											vector<string> scanned_items;
											for (int i = 0; i < world->droppedItems.size(); i++) {
												int id = world->droppedItems.at(i).id;
												if (getItemDef(id).blockType == BlockTypes::LOCK) {
													if (scanned_items.size() != 0) {
														if (find(scanned_items.begin(), scanned_items.end(), to_string(id)) != scanned_items.end()) continue;
													}
													scanned_items.push_back(to_string(id));
													int count = 0;
													for (int is = 0; is < world->droppedItems.size(); is++) {
														if (world->droppedItems.at(is).id == id) {
															count += world->droppedItems.at(is).count;
														}
													}
													additems += "" + to_string(id) + "," + to_string(count) + ",";
												}
											}
											if (additems == "") Player::OnDialogRequest(peer, "\nset_default_color|`o\nadd_label_with_icon|big|`wFloating Items``|left|6016|\nadd_spacer|small|\nadd_textbox|No items found.|left|\nadd_spacer|small|\nadd_button|backToFloatingItems|Back|noflags|0|0|\nend_dialog|statsblock|||");
											else Player::OnDialogRequest(peer, "set_default_color|`o\nadd_label_with_icon|big|`wFloating Items``|left|6016|\nadd_spacer|small|\nadd_button|backToFloatingItems|Back|noflags|0|0|\nadd_quick_exit|\nadd_textbox|`wItem List(" + to_string(scanned_items.size()) + "):|left|\nadd_textbox|Press on the icon to be directed to the location of the item.|left|\nadd_spacer|small|\nadd_label_with_icon_button_list|small|`w%s : %s|left|findObject_|itemID_itemAmount|" + additems + "\nadd_spacer|small|\nadd_textbox|`wItem List(" + to_string(scanned_items.size()) + "):|left|\nadd_button|backToFloatingItems|Back|noflags|0|0|\nend_dialog|statsblock|||");
											break;
										} if (infoDat.at(1) == "filterBySeedsInFloatingItems") {
											string additems = "";
											int totalfloatinggems = 0;
											vector<string> scanned_items;
											for (int i = 0; i < world->droppedItems.size(); i++) {
												int id = world->droppedItems.at(i).id;
												if (getItemDef(id).blockType == BlockTypes::SEED) {
													if (scanned_items.size() != 0) {
														if (find(scanned_items.begin(), scanned_items.end(), to_string(id)) != scanned_items.end()) continue;
													}
													scanned_items.push_back(to_string(id));
													int count = 0;
													for (int is = 0; is < world->droppedItems.size(); is++) {
														if (world->droppedItems.at(is).id == id) {
															count += world->droppedItems.at(is).count;
														}
													}
													additems += "" + to_string(id) + "," + to_string(count) + ",";
												}
											}
											if (additems == "") Player::OnDialogRequest(peer, "\nset_default_color|`o\nadd_label_with_icon|big|`wFloating Items``|left|6016|\nadd_spacer|small|\nadd_textbox|No items found.|left|\nadd_spacer|small|\nadd_button|backToFloatingItems|Back|noflags|0|0|\nend_dialog|statsblock|||");
											else Player::OnDialogRequest(peer, "set_default_color|`o\nadd_label_with_icon|big|`wFloating Items``|left|6016|\nadd_spacer|small|\nadd_button|backToFloatingItems|Back|noflags|0|0|\nadd_quick_exit|\nadd_textbox|`wItem List(" + to_string(scanned_items.size()) + "):|left|\nadd_textbox|Press on the icon to be directed to the location of the item.|left|\nadd_spacer|small|\nadd_label_with_icon_button_list|small|`w%s : %s|left|findObject_|itemID_itemAmount|" + additems + "\nadd_spacer|small|\nadd_textbox|`wItem List(" + to_string(scanned_items.size()) + "):|left|\nadd_button|backToFloatingItems|Back|noflags|0|0|\nend_dialog|statsblock|||");
											break;
										} if (infoDat.at(1) == "filterByBlocksInFloatingItems") {
											string additems = "";
											int totalfloatinggems = 0;
											vector<string> scanned_items;
											for (int i = 0; i < world->droppedItems.size(); i++) {
												int id = world->droppedItems.at(i).id;
												if (getItemDef(id).blockType == BlockTypes::FOREGROUND || getItemDef(id).blockType == BlockTypes::BACKGROUND || getItemDef(id).blockType == BlockTypes::SFX_FOREGROUND || getItemDef(id).blockType == BlockTypes::BEDROCK || getItemDef(id).blockType == BlockTypes::PAIN_BLOCK) {
													if (scanned_items.size() != 0) {
														if (find(scanned_items.begin(), scanned_items.end(), to_string(id)) != scanned_items.end()) continue;
													}
													scanned_items.push_back(to_string(id));
													int count = 0;
													for (int is = 0; is < world->droppedItems.size(); is++) {
														if (world->droppedItems.at(is).id == id) {
															count += world->droppedItems.at(is).count;
														}
													}
													additems += "" + to_string(id) + "," + to_string(count) + ",";
												}
											}
											if (additems == "") Player::OnDialogRequest(peer, "\nset_default_color|`o\nadd_label_with_icon|big|`wFloating Items``|left|6016|\nadd_spacer|small|\nadd_textbox|No items found.|left|\nadd_spacer|small|\nadd_button|backToFloatingItems|Back|noflags|0|0|\nend_dialog|statsblock|||");
											else Player::OnDialogRequest(peer, "set_default_color|`o\nadd_label_with_icon|big|`wFloating Items``|left|6016|\nadd_spacer|small|\nadd_button|backToFloatingItems|Back|noflags|0|0|\nadd_quick_exit|\nadd_textbox|`wItem List(" + to_string(scanned_items.size()) + "):|left|\nadd_textbox|Press on the icon to be directed to the location of the item.|left|\nadd_spacer|small|\nadd_label_with_icon_button_list|small|`w%s : %s|left|findObject_|itemID_itemAmount|" + additems + "\nadd_spacer|small|\nadd_textbox|`wItem List(" + to_string(scanned_items.size()) + "):|left|\nadd_button|backToFloatingItems|Back|noflags|0|0|\nend_dialog|statsblock|||");
											break;
										} if (infoDat.at(1) == "filterByClothingInFloatingItems") {
											string additems = "";
											int totalfloatinggems = 0;
											vector<string> scanned_items;
											for (int i = 0; i < world->droppedItems.size(); i++) {
												int id = world->droppedItems.at(i).id;
												if (getItemDef(id).blockType == BlockTypes::CLOTHING) {
													if (scanned_items.size() != 0) {
														if (find(scanned_items.begin(), scanned_items.end(), to_string(id)) != scanned_items.end()) continue;
													}
													scanned_items.push_back(to_string(id));
													int count = 0;
													for (int is = 0; is < world->droppedItems.size(); is++) {
														if (world->droppedItems.at(is).id == id) {
															count += world->droppedItems.at(is).count;
														}
													}
													additems += "" + to_string(id) + "," + to_string(count) + ",";
												}
											}
											if (additems == "") Player::OnDialogRequest(peer, "\nset_default_color|`o\nadd_label_with_icon|big|`wFloating Items``|left|6016|\nadd_spacer|small|\nadd_textbox|No items found.|left|\nadd_spacer|small|\nadd_button|backToFloatingItems|Back|noflags|0|0|\nend_dialog|statsblock|||");
											else Player::OnDialogRequest(peer, "set_default_color|`o\nadd_label_with_icon|big|`wFloating Items``|left|6016|\nadd_spacer|small|\nadd_button|backToFloatingItems|Back|noflags|0|0|\nadd_quick_exit|\nadd_textbox|`wItem List(" + to_string(scanned_items.size()) + "):|left|\nadd_textbox|Press on the icon to be directed to the location of the item.|left|\nadd_spacer|small|\nadd_label_with_icon_button_list|small|`w%s : %s|left|findObject_|itemID_itemAmount|" + additems + "\nadd_spacer|small|\nadd_textbox|`wItem List(" + to_string(scanned_items.size()) + "):|left|\nadd_button|backToFloatingItems|Back|noflags|0|0|\nend_dialog|statsblock|||");
											break;
										} if (infoDat.at(1) == "filterByConsumablesInFloatingItems") {
											string additems = "";
											int totalfloatinggems = 0;
											vector<string> scanned_items;
											for (int i = 0; i < world->droppedItems.size(); i++) {
												int id = world->droppedItems.at(i).id;
												if (getItemDef(id).blockType == BlockTypes::CONSUMABLE) {
													if (scanned_items.size() != 0) {
														if (find(scanned_items.begin(), scanned_items.end(), to_string(id)) != scanned_items.end()) continue;
													}
													scanned_items.push_back(to_string(id));
													int count = 0;
													for (int is = 0; is < world->droppedItems.size(); is++) {
														if (world->droppedItems.at(is).id == id) {
															count += world->droppedItems.at(is).count;
														}
													}
													additems += "" + to_string(id) + "," + to_string(count) + ",";
												}
											}
											if (additems == "") Player::OnDialogRequest(peer, "\nset_default_color|`o\nadd_label_with_icon|big|`wFloating Items``|left|6016|\nadd_spacer|small|\nadd_textbox|No items found.|left|\nadd_spacer|small|\nadd_button|backToFloatingItems|Back|noflags|0|0|\nend_dialog|statsblock|||");
											else Player::OnDialogRequest(peer, "set_default_color|`o\nadd_label_with_icon|big|`wFloating Items``|left|6016|\nadd_spacer|small|\nadd_button|backToFloatingItems|Back|noflags|0|0|\nadd_quick_exit|\nadd_textbox|`wItem List(" + to_string(scanned_items.size()) + "):|left|\nadd_textbox|Press on the icon to be directed to the location of the item.|left|\nadd_spacer|small|\nadd_label_with_icon_button_list|small|`w%s : %s|left|findObject_|itemID_itemAmount|" + additems + "\nadd_spacer|small|\nadd_textbox|`wItem List(" + to_string(scanned_items.size()) + "):|left|\nadd_button|backToFloatingItems|Back|noflags|0|0|\nend_dialog|statsblock|||");
											break;
										} if (infoDat.at(1) == "searchInFloatingItems") {
											string additems = "";
											int totalfloatinggems = 0;
											vector<string> scanned_items;
											for (int i = 0; i < world->droppedItems.size(); i++) {
												int id = world->droppedItems.at(i).id;
												if (scanned_items.size() != 0) {
													if (find(scanned_items.begin(), scanned_items.end(), to_string(id)) != scanned_items.end()) continue;
												}
												scanned_items.push_back(to_string(id));
												int count = 0;
												for (int is = 0; is < world->droppedItems.size(); is++) {
													if (world->droppedItems.at(is).id == id) {
														count += world->droppedItems.at(is).count;
													}
												}
												additems += "" + to_string(id) + "," + to_string(count) + ",";
											}
											if (additems == "") Player::OnDialogRequest(peer, "\nset_default_color|`o\nadd_label_with_icon|big|`wFloating Items``|left|6016|\nadd_spacer|small|\nadd_textbox|No items found.|left|\nadd_spacer|small|\nadd_button|backToFloatingItems|Back|noflags|0|0|\nend_dialog|statsblock|||");
											else Player::OnDialogRequest(peer, "set_default_color|`o\nadd_label_with_icon|big|`wFloating Items``|left|6016|\nadd_spacer|small|\nadd_button|backToFloatingItems|Back|noflags|0|0|\nadd_quick_exit|\nadd_textbox|`wItem List(" + to_string(scanned_items.size()) + "):|left|\nadd_textbox|Press on the icon to be directed to the location of the item.|left|\nadd_spacer|small|\nadd_label_with_icon_button_list|small|`w%s : %s|left|findObject_|itemID_itemAmount|" + additems + "\nadd_spacer|small|\nadd_textbox|`wItem List(" + to_string(scanned_items.size()) + "):|left|\nadd_button|backToFloatingItems|Back|noflags|0|0|\nend_dialog|statsblock|||");
											break;
										}
									}
									if (isSongDialog) {
										if (infoDat.at(0) == "checkbox_ahrix") {
											if (infoDat.at(1) == "1") {
												Player::OnConsoleMessage(peer, "Playing now `$Ahrix Nova");
												Player::PlayAudio(peer, "audio/ogg/ahrix_nova.ogg", 0);
												break;
											}
										} if (infoDat.at(0) == "checkbox_phut") {
											if (infoDat.at(1) == "1") {
												Player::OnConsoleMessage(peer, "Playing now `$Phut Hon");
												Player::PlayAudio(peer, "audio/ogg/phut_hon.ogg", 0);
												break;
											}
										} if (infoDat.at(0) == "checkbox_stopme") {
											if (infoDat.at(1) == "1") {
												Player::OnConsoleMessage(peer, "Playing now `$Dont't stop me now");
												Player::PlayAudio(peer, "audio/ogg/dont_stop_me_now.ogg", 0);
												break;
											}
										} if (infoDat.at(0) == "checkbox_feelit") {
											if (infoDat.at(1) == "1") {
												Player::OnConsoleMessage(peer, "Playing now `$Can you feel it");
												Player::PlayAudio(peer, "audio/ogg/feel_it.ogg", 0);
												break;
											}
										} if (infoDat.at(0) == "checkbox_sayso") {
											if (infoDat.at(1) == "1") {
												Player::OnConsoleMessage(peer, "Playing now `$Say so");
												Player::PlayAudio(peer, "audio/ogg/say_so.ogg", 0);
												break;
											}
										} if (infoDat.at(0) == "checkbox_bealone") {
											if (infoDat.at(1) == "1") {
												Player::OnConsoleMessage(peer, "Playing now `$Never be alone");
												Player::PlayAudio(peer, "audio/ogg/be_alone.ogg", 0);
												break;
											}
										}
									}
									if (isRolesDialog) {
										if (infoDat.at(1) == "back") {
											int firefighterxp = 1500;
											if (pData->firefighterlevel > 0) firefighterxp = firefighterxp * pData->firefighterlevel;
											if (pData->firefighterlevel == 0) firefighterxp = 750;
											int providerxp = 1300;
											if (pData->providerlevel > 0) providerxp = providerxp * pData->providerlevel;
											if (pData->providerlevel == 0) providerxp = 600;
											int geigerxp = 600;
											if (pData->geigerlevel > 0) geigerxp = geigerxp * pData->geigerlevel;
											if (pData->geigerlevel == 0) geigerxp = 350;
											int fishermanxp = 1600;
											if (pData->fishermanlevel > 0) fishermanxp = fishermanxp * pData->fishermanlevel;
											if (pData->fishermanlevel == 0) fishermanxp = 900;
											Player::OnDialogRequest(peer, "add_label_with_icon|big|`wRole Stats``|left|982|\nadd_spacer|small|\nadd_textbox|`9What prizes and powers have you unlocked in your Roles and what's left to discover? Find out here!``|left|\nadd_spacer|small|\nadd_player_info|Firefighter|" + to_string(pData->firefighterlevel) + "|" + to_string(pData->firefighterxp) + "|" + to_string(firefighterxp) + "|\nadd_spacer|small|\nadd_button|viewFirefighter|`0View Firefighter Rewards``|noflags|0|0|\nadd_spacer|small|\nadd_player_info|Provider|" + to_string(pData->providerlevel) + "|" + to_string(pData->providerxp) + "|" + to_string(providerxp) + "|\nadd_spacer|small|\nadd_button|viewProvider|`0View Provider Rewards``|noflags|0|0|\nadd_spacer|small|\nadd_player_info|Geiger Hunter|" + to_string(pData->geigerlevel) + "|" + to_string(pData->geigerxp) + "|" + to_string(geigerxp) + "|\nadd_spacer|small|\nadd_button|viewGeiger|`0View Geiger Hunter Rewards``|noflags|0|0|\nadd_spacer|small|\nadd_player_info|Fisherman|" + to_string(pData->fishermanlevel) + "|" + to_string(pData->fishermanxp) + "|" + to_string(fishermanxp) + "|\nadd_spacer|small|\nadd_button|viewFisherman|`0View Fisherman Rewards``|noflags|0|0|\nadd_spacer|small|\nadd_button|back|Back|noflags|0|0|\nend_dialog|mainwrenchpage|||\nadd_quick_exit|");
											break;
										}
									}
									if (isWrenchDialog) {
										if (infoDat.at(1) == "back") {
											if (pData->haveGrowId == true) {
												send_info(peer, pData);
											}
											else
											{
												SendRegisterDialog(peer);
												enet_host_flush(server);
											}
											break;
										}
									}
									if (isPassDoorDialog) {
										if (infoDat.at(0) == "password") {
											toUpperCase(infoDat.at(1));
											if (infoDat.at(1) != world->items.at(pData->wrenchsession).password) {
												Player::OnTalkBubble(peer, pData->netID, "`4Wrong password!", 0, false);
												break;
											}
											else {
												Player::OnTalkBubble(peer, pData->netID, "`2The door opens!", 0, false);
												DoEnterDoor(peer, world, pData->lastPunchX, pData->lastPunchY, false);
												break;
											}
										}
									}
									if (worldcategoryDialog) {
										if (infoDat.at(1) != "worldcategory") {
											if (world->category == infoDat.at(1)) {
												Player::OnTalkBubble(peer, pData->netID, "Your world is already in that category!", 0, false);
											}
											else {
												if (world->category == "Guild") {
													break;
												}
												if (infoDat.at(1) != "Mining" && infoDat.at(1) != "Adventure" && infoDat.at(1) != "Art" && infoDat.at(1) != "Farm" && infoDat.at(1) != "Game" && infoDat.at(1) != "Information" && infoDat.at(1) != "Music" && infoDat.at(1) != "Parkour" && infoDat.at(1) != "Puzzle" && infoDat.at(1) != "Roleplay" && infoDat.at(1) != "Shop" && infoDat.at(1) != "Social" && infoDat.at(1) != "Storage" && infoDat.at(1) != "Story" && infoDat.at(1) != "Trade") break;
												world->category = infoDat.at(1);
												Player::OnConsoleMessage(peer, "This world has been moved to the '" + infoDat.at(1) + "' category! Everyone, please type `2/rate`` to rate it from 1-5 stars.");
											}
											break;
										}
									}
									if (isAutoPlaceDialog) {
										if (infoDat.at(0) == "autoplaceitem") {
											int x = pData->lastPunchX;
											int y = pData->lastPunchY;
											if (world->items.at(x + (y * world->width)).mid == atoi(infoDat.at(1).c_str())) break;
											if (world->items.at(x + (y * world->width)).mc != 0) {
												Player::OnTalkBubble(peer, pData->netID, "Empty the machine first!", 0, true);
												break;
											}
											auto contains = false;
											SearchInventoryItem(peer, atoi(infoDat.at(1).c_str()), 1, contains);
											if (!contains) break;
											if (getItemDef(atoi(infoDat.at(1).c_str())).blockType == BlockTypes::FOREGROUND || isSeed(atoi(infoDat.at(1).c_str())) || getItemDef(atoi(infoDat.at(1).c_str())).blockType == BlockTypes::SFX_FOREGROUND) {
												world->items.at(x + (y * world->width)).mid = atoi(infoDat.at(1).c_str());
												for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
													if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
													if (isHere(peer, currentPeer)) {
														send_item_sucker(currentPeer, 6954, x, y, world->items.at(x + (y * world->width)).mid, -1, true, true, world->items.at(x + (y * world->width)).background);
													}
												}
											}
											else {
												Player::OnTalkBubble(peer, pData->netID, "You cannot use this item in Techno-Organic Engine!", 0, true);
											}
										}
										if (infoDat.at(0) == "autoplaceidentityitem") {
											int x = pData->lastPunchX;
											int y = pData->lastPunchY;
											if (world->items.at(x + (y * world->width)).vid == atoi(infoDat.at(1).c_str())) break;
											auto contains = false;
											SearchInventoryItem(peer, atoi(infoDat.at(1).c_str()), 1, contains);
											if (!contains) break;
											if (getItemDef(atoi(infoDat.at(1).c_str())).blockType == BlockTypes::FOREGROUND || getItemDef(atoi(infoDat.at(1).c_str())).blockType == BlockTypes::PLATFORM || getItemDef(atoi(infoDat.at(1).c_str())).blockType == BlockTypes::SFX_FOREGROUND) {
												world->items.at(x + (y * world->width)).vid = atoi(infoDat.at(1).c_str());
											}
											else {
												Player::OnTalkBubble(peer, pData->netID, "You cannot use this item as identification block in Techno-Organic Engine!", 0, true);
											}
										}
										if (infoDat.at(0) == "checkbox_publicremote") {
											int x = pData->lastPunchX;
											int y = pData->lastPunchY;
											if (infoDat.at(1) == "1") world->items.at(x + (y * world->width)).rm = true;
											else if (infoDat.at(1) == "0") world->items.at(x + (y * world->width)).rm = false;
										}
									}
									if (isAutoBreakDialog) {
										if (infoDat.at(0) == "autobreakitem") {
											int x = pData->lastPunchX;
											int y = pData->lastPunchY;
											if (world->items.at(x + (y * world->width)).mid == atoi(infoDat.at(1).c_str())) break;
											auto contains = false;
											SearchInventoryItem(peer, atoi(infoDat.at(1).c_str()), 1, contains);
											if (!contains) break;
											if (isSeed(atoi(infoDat.at(1).c_str())) && !world->items.at(x + (y * world->width)).rm) {
												Player::OnTalkBubble(peer, pData->netID, "You will need to enable auto harvest trees mode, before selecting the seed!", 0, true);
												break;
											}
											if (getItemDef(atoi(infoDat.at(1).c_str())).blockType == BlockTypes::FOREGROUND || isSeed(atoi(infoDat.at(1).c_str())) || getItemDef(atoi(infoDat.at(1).c_str())).blockType == BlockTypes::PLATFORM || getItemDef(atoi(infoDat.at(1).c_str())).blockType == BlockTypes::BACKGROUND || getItemDef(atoi(infoDat.at(1).c_str())).blockType == BlockTypes::SFX_FOREGROUND) {
												world->items.at(x + (y * world->width)).mid = atoi(infoDat.at(1).c_str());
												for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
													if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
													if (isHere(peer, currentPeer)) {
														send_item_sucker(currentPeer, 6952, x, y, world->items.at(x + (y * world->width)).mid, 0, false, false, world->items.at(x + (y * world->width)).background);
													}
												}
											}
											else {
												Player::OnTalkBubble(peer, pData->netID, "You cannot use this item in Tesseract Manipulator!", 0, true);
											}
											break;
										}
										if (infoDat.at(0) == "checkbox_autoharvest") {
											int x = pData->lastPunchX;
											int y = pData->lastPunchY;
											if (world->items.at(x + (y * world->width)).rm && infoDat.at(1) == "1") break;
											if (!world->items.at(x + (y * world->width)).rm && infoDat.at(1) == "0") break;
											if (world->items.at(x + (y * world->width)).rm && isSeed(world->items.at(x + (y * world->width)).mid)) {
												Player::OnTalkBubble(peer, pData->netID, "You will need to stop the machine before disabling auto harvest mode!", 0, true);
												break;
											}
											if (infoDat.at(1) == "1") world->items.at(x + (y * world->width)).rm = true;
											else if (infoDat.at(1) == "0") world->items.at(x + (y * world->width)).rm = false;
										}
									}
									if (vending) {
										if (infoDat.at(1) == "pullstock") break;
										if (infoDat.at(0) == "chk_peritem") {
											per_item = infoDat.at(1);
										}
										if (infoDat.at(0) == "chk_perlock") {
											if (world->owner != "" && !isWorldOwner(peer, world) && !mod(peer)) break;
											int x = pData->lastPunchX;
											int y = pData->lastPunchY;
											if (world->items.at(x + (y * world->width)).foreground != 2978) break;
											bool Changedmh = false;
											if (per_item == "1" && infoDat.at(1) == "1") {
												Player::OnConsoleMessage(peer, "You can't have it both ways.");
												Player::OnTalkBubble(peer, pData->netID, "You can't have it both ways.", 0, true);
												break;
											}
											if (!world->items.at(x + (y * world->width)).opened && per_item == "0" && world->items.at(x + (y * world->width)).rm && infoDat.at(1) != "1") {
												Player::OnConsoleMessage(peer, "You need to pick a pricing method.");
												Player::OnTalkBubble(peer, pData->netID, "You need to pick a pricing method.", 0, true);
												break;
											}
											if (per_item == "0" && world->items.at(x + (y * world->width)).rm && world->items.at(x + (y * world->width)).vprice > world->items.at(x + (y * world->width)).vcount && world->items.at(x + (y * world->width)).vid != 0) {
												Player::OnConsoleMessage(peer, "You don't have enough in there for anyone to buy at that price!");
												Player::OnTalkBubble(peer, pData->netID, "You don't have enough in there for anyone to buy at that price!", 0, true);
												break;
											}
											if (per_item == "1" && !world->items.at(x + (y * world->width)).rm) {
												Changedmh = true;
												world->items.at(x + (y * world->width)).rm = true;
												auto islocks = false;
												if (world->items.at(x + (y * world->width)).vdraw >= 1) islocks = true;
												for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
													if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
													if (isHere(peer, currentPeer)) {
														if (world->items.at(x + (y * world->width)).opened && world->items.at(x + (y * world->width)).vcount < world->items.at(x + (y * world->width)).vprice) {
															UpdateVend(currentPeer, x, y, 0, islocks, world->items.at(x + (y * world->width)).vprice, world->items.at(x + (y * world->width)).background, world->items.at(x + (y * world->width)).opened);
														}
														else UpdateVend(currentPeer, x, y, world->items.at(x + (y * world->width)).vid, islocks, world->items.at(x + (y * world->width)).vprice, world->items.at(x + (y * world->width)).background, world->items.at(x + (y * world->width)).opened);
													}
												}
											}
											else if (per_item == "0" && world->items.at(x + (y * world->width)).rm) {
												Changedmh = true;
												world->items.at(x + (y * world->width)).rm = false;
												auto islocks = false;
												if (world->items.at(x + (y * world->width)).vdraw >= 1) islocks = true;
												for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
													if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
													if (isHere(peer, currentPeer)) {
														if (world->items.at(x + (y * world->width)).opened && world->items.at(x + (y * world->width)).vcount < world->items.at(x + (y * world->width)).vprice) {
															UpdateVend(currentPeer, x, y, 0, islocks, world->items.at(x + (y * world->width)).vprice, world->items.at(x + (y * world->width)).background, world->items.at(x + (y * world->width)).opened);
														}
														else UpdateVend(currentPeer, x, y, world->items.at(x + (y * world->width)).vid, islocks, world->items.at(x + (y * world->width)).vprice, world->items.at(x + (y * world->width)).background, world->items.at(x + (y * world->width)).opened);
														//Player::OnParticleEffect(currentPeer, 44, x * 32, y * 32, 0);
													}
												}
											}
											if (!world->items.at(x + (y * world->width)).rm && infoDat.at(1) == "0" && world->items.at(x + (y * world->width)).opened) {
												Player::OnConsoleMessage(peer, "You need to pick a pricing method.");
												Player::OnTalkBubble(peer, pData->netID, "You need to pick a pricing method.", 0, true);
												break;
											}
											if (infoDat.at(1) == "1" && !world->items.at(x + (y * world->width)).opened) {
												//cout << "price change" << endl;
												Changedmh = true;
												world->items.at(x + (y * world->width)).opened = true;
												auto islocks = false;
												if (world->items.at(x + (y * world->width)).vdraw >= 1) islocks = true;
												for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
													if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
													if (isHere(peer, currentPeer)) {
														if (world->items.at(x + (y * world->width)).opened && world->items.at(x + (y * world->width)).vcount < world->items.at(x + (y * world->width)).vprice) {
															UpdateVend(currentPeer, x, y, 0, islocks, world->items.at(x + (y * world->width)).vprice, world->items.at(x + (y * world->width)).background, world->items.at(x + (y * world->width)).opened);
														}
														else UpdateVend(currentPeer, x, y, world->items.at(x + (y * world->width)).vid, islocks, world->items.at(x + (y * world->width)).vprice, world->items.at(x + (y * world->width)).background, world->items.at(x + (y * world->width)).opened);
														//Player::OnParticleEffect(currentPeer, 44, x * 32, y * 32, 0);
													}
												}
											}
											else if (infoDat.at(1) == "0" && world->items.at(x + (y * world->width)).opened) {
												Changedmh = true;
												world->items.at(x + (y * world->width)).opened = false;
												auto islocks = false;
												if (world->items.at(x + (y * world->width)).vdraw >= 1) islocks = true;
												for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
													if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
													if (isHere(peer, currentPeer)) {
														if (world->items.at(x + (y * world->width)).opened && world->items.at(x + (y * world->width)).vcount < world->items.at(x + (y * world->width)).vprice) {
															UpdateVend(currentPeer, x, y, 0, islocks, world->items.at(x + (y * world->width)).vprice, world->items.at(x + (y * world->width)).background, world->items.at(x + (y * world->width)).opened);
														}
														else UpdateVend(currentPeer, x, y, world->items.at(x + (y * world->width)).vid, islocks, world->items.at(x + (y * world->width)).vprice, world->items.at(x + (y * world->width)).background, world->items.at(x + (y * world->width)).opened);
													}
												}
											}
											if (Changedmh) {
												for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
													if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
													if (isHere(peer, currentPeer)) {
														if (world->items.at(x + (y * world->width)).opened) {
															Player::OnConsoleMessage(currentPeer, "`7[`o" + pData->displayName + " `ochanged the price of `2" + getItemDef(world->items.at(x + (y * world->width)).vid).name + "`o to `6" + to_string(setprice) + " per World lock.`7]");
															Player::OnTalkBubble(currentPeer, pData->netID, "`7[`w" + pData->displayName + " `wchanged the price of `2" + getItemDef(world->items.at(x + (y * world->width)).vid).name + "`w to `6" + to_string(setprice) + " per World lock.`7]", 0, true);
														}
														else {
															Player::OnConsoleMessage(currentPeer, "`7[`o" + pData->displayName + " `ochanged the price of `2" + getItemDef(world->items.at(x + (y * world->width)).vid).name + "`o to `5" + to_string(setprice) + " World Locks each.`7]");
															Player::OnTalkBubble(currentPeer, pData->netID, "`7[`w" + pData->displayName + " `wchanged the price of `2" + getItemDef(world->items.at(x + (y * world->width)).vid).name + "`w to `5" + to_string(setprice) + " World Locks each.`7]", 0, true);
														}
														Player::OnParticleEffect(currentPeer, 44, x * 32, y * 32, 0);
													}
												}
											}
											if (world->owner != "" && !isWorldOwner(peer, world) && !mod(peer)) break;
											string setpricestr = vend_set_price;
											if (setpricestr.size() > 5 || setpricestr.size() <= 0) break;
											setprice = atoi(setpricestr.c_str());
											if (setprice <= 0) continue;
											if (setprice > 20000) {
												Player::OnTalkBubble(peer, pData->netID, "Well, that's a silly price.", 0, true);
												break;
											}
											if (world->items.at(x + (y * world->width)).foreground != 2978) break;
											if (world->items.at(x + (y * world->width)).opened && setprice > world->items.at(x + (y * world->width)).vcount && world->items.at(x + (y * world->width)).vid != 0) {
												Player::OnConsoleMessage(peer, "You don't have enough in there for anyone to buy at that price!");
												Player::OnTalkBubble(peer, pData->netID, "You don't have enough in there for anyone to buy at that price!", 0, true);
												break;
											}
											if (world->items.at(x + (y * world->width)).opened && setprice > 250 && world->items.at(x + (y * world->width)).vid != 0) {
												Player::OnConsoleMessage(peer, "That price is so low, nobody can carry a World Lock worth of items!");
												Player::OnTalkBubble(peer, pData->netID, "That price is so low, nobody can carry a World Lock worth of items!", 0, true);
												break;
											}
											if (setprice == world->items.at(x + (y * world->width)).vprice) continue;
											world->items.at(x + (y * world->width)).vprice = setprice;
											auto islocks = false;
											if (world->items.at(x + (y * world->width)).vdraw >= 1) islocks = true;
											ENetPeer* currentPeer;
											for (currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
												if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
												if (isHere(peer, currentPeer)) {
													if (!Changedmh) {
														if (world->items.at(x + (y * world->width)).opened) {
															Player::OnConsoleMessage(currentPeer, "`7[`o" + pData->displayName + " `ochanged the price of `2" + getItemDef(world->items.at(x + (y * world->width)).vid).name + "`o to `6" + to_string(setprice) + " per World lock.`7]");
															Player::OnTalkBubble(currentPeer, pData->netID, "`7[`w" + pData->displayName + " `wchanged the price of `2" + getItemDef(world->items.at(x + (y * world->width)).vid).name + "`w to `6" + to_string(setprice) + " per World lock.`7]", 0, true);
														}
														else {
															Player::OnConsoleMessage(currentPeer, "`7[`o" + pData->displayName + " `ochanged the price of `2" + getItemDef(world->items.at(x + (y * world->width)).vid).name + "`o to `5" + to_string(setprice) + " World Locks each.`7]");
															Player::OnTalkBubble(currentPeer, pData->netID, "`7[`w" + pData->displayName + " `wchanged the price of `2" + getItemDef(world->items.at(x + (y * world->width)).vid).name + "`w to `5" + to_string(setprice) + " World Locks each.`7]", 0, true);
														}
														Player::OnParticleEffect(currentPeer, 44, x * 32, y * 32, 0);
													}
													if (world->items.at(x + (y * world->width)).opened && world->items.at(x + (y * world->width)).vcount < world->items.at(x + (y * world->width)).vprice) {
														UpdateVend(currentPeer, x, y, 0, islocks, setprice, world->items.at(x + (y * world->width)).background, world->items.at(x + (y * world->width)).opened);
													}
													else UpdateVend(currentPeer, x, y, world->items.at(x + (y * world->width)).vid, islocks, setprice, world->items.at(x + (y * world->width)).background, world->items.at(x + (y * world->width)).opened);
												}
											}
											break;
										}
										if (infoDat.at(0) == "stockitem") {
											if (world->owner != "" && !isWorldOwner(peer, world) && !mod(peer)) break;
											string stockitemstr = infoDat.at(1);
											if (stockitemstr.size() > 5 || stockitemstr.size() <= 0) break;
											stockitem = atoi(stockitemstr.c_str());
											if (stockitem < 0 || stockitem > maxItems) break;
											if (stockitem == 1424 || stockitem == 18 || stockitem == 32 || stockitem == 6336 || getItemDef(stockitem).blockType == BlockTypes::LOCK || getItemDef(stockitem).properties & Property_Untradable || getItemDef(stockitem).blockType == BlockTypes::FISH || stockitem == 8552 || stockitem == 9472 || stockitem == 9482 || stockitem == 9356 || stockitem == 9492 || stockitem == 9498 || stockitem == 8774 || stockitem == 1790 || stockitem == 2592 || stockitem == 1784 || stockitem == 1792 || stockitem == 1794 || stockitem == 7734 || stockitem == 8306 || stockitem == 9458) {
												Player::OnConsoleMessage(peer, "`oCan't put that in a Vending Machine.");
												Player::OnTalkBubble(peer, pData->netID, "`wCan't put that in a Vending Machine.", 0, true);
												break;
											}
											int count = 1;
											for (auto i = 0; i < pData->inventory.items.size(); i++) {
												if (pData->inventory.items.at(i).itemID == stockitem) {
													count = pData->inventory.items.at(i).itemCount;
													break;
												}
											}
											if (stockitem == 0 || count == 0) break;
											auto contains = false;
											SearchInventoryItem(peer, stockitem, count, contains);
											if (!contains) break;
											RemoveInventoryItem(stockitem, count, peer, true);
											updateplayerset(peer, stockitem);
											int x = pData->lastPunchX;
											int y = pData->lastPunchY;
											if (world->items.at(x + (y * world->width)).foreground != 2978) break;
											world->items.at(x + (y * world->width)).vprice = setprice;
											world->items.at(x + (y * world->width)).vid = stockitem;
											world->items.at(x + (y * world->width)).vcount = count;
											auto islocks = false;
											if (world->items.at(x + (y * world->width)).vdraw >= 1) islocks = true;
											ENetPeer* currentPeer;
											for (currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
												if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
												if (isHere(peer, currentPeer)) {
													Player::OnConsoleMessage(currentPeer, "`7[`o" + pData->displayName + " `oput `2" + getItemDef(stockitem).name + " `oin the Vending Machine.`7]");
													Player::OnTalkBubble(currentPeer, pData->netID, "`7[`w" + pData->displayName + " `wput `2" + getItemDef(stockitem).name + "`w in the Vending Machine.`7]", 0, false);
													if (world->items.at(x + (y * world->width)).opened && world->items.at(x + (y * world->width)).vcount < world->items.at(x + (y * world->width)).vprice) {
														UpdateVend(currentPeer, x, y, 0, islocks, setprice, world->items.at(x + (y * world->width)).background, world->items.at(x + (y * world->width)).opened);
													}
													else UpdateVend(currentPeer, x, y, stockitem, islocks, setprice, world->items.at(x + (y * world->width)).background, world->items.at(x + (y * world->width)).opened);
													Player::OnParticleEffect(currentPeer, 44, x * 32, y * 32, 0);
												}
											}
											SendVendDialog(peer, world);
											break;
										}
										if (infoDat.at(0) == "setprice") {
											if (world->owner != "" && !isWorldOwner(peer, world) && !mod(peer)) break;
											string setpricestr = infoDat.at(1);
											if (setpricestr.size() > 5 || setpricestr.size() <= 0) break;
											setprice = atoi(setpricestr.c_str());
											if (setprice <= 0) continue;
											if (setprice > 20000) {
												Player::OnTalkBubble(peer, pData->netID, "Well, that's a silly price.", 0, true);
												break;
											}
											int x = pData->lastPunchX;
											int y = pData->lastPunchY;
											if (world->items.at(x + (y * world->width)).foreground != 2978) break;
											if (vend_set_price == "0") {
												vend_set_price = to_string(setprice);
												continue;
											}
											if (world->items.at(x + (y * world->width)).opened && setprice > world->items.at(x + (y * world->width)).vcount && world->items.at(x + (y * world->width)).vid != 0) {
												Player::OnConsoleMessage(peer, "You don't have enough in there for anyone to buy at that price!");
												Player::OnTalkBubble(peer, pData->netID, "You don't have enough in there for anyone to buy at that price!", 0, true);
												break;
											}
											if (world->items.at(x + (y * world->width)).opened && setprice > 250 && world->items.at(x + (y * world->width)).vid != 0) {
												Player::OnConsoleMessage(peer, "That price is so low, nobody can carry a World Lock worth of items!");
												Player::OnTalkBubble(peer, pData->netID, "That price is so low, nobody can carry a World Lock worth of items!", 0, true);
												break;
											}
											if (setprice == world->items.at(x + (y * world->width)).vprice) continue;
											world->items.at(x + (y * world->width)).vprice = setprice;
											auto islocks = false;
											if (world->items.at(x + (y * world->width)).vdraw >= 1) islocks = true;
											ENetPeer* currentPeer;
											for (currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
												if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
												if (isHere(peer, currentPeer)) {
													//cout << "say6" << endl;
													if (world->items.at(x + (y * world->width)).opened) {
														Player::OnConsoleMessage(currentPeer, "`7[`o" + pData->displayName + " `ochanged the price of `2" + getItemDef(world->items.at(x + (y * world->width)).vid).name + "`o to `6" + to_string(setprice) + " per World lock.`7]");
														Player::OnTalkBubble(currentPeer, pData->netID, "`7[`w" + pData->displayName + " `wchanged the price of `2" + getItemDef(world->items.at(x + (y * world->width)).vid).name + "`w to `6" + to_string(setprice) + " per World lock.`7]", 0, true);
													}
													else {
														Player::OnConsoleMessage(currentPeer, "`7[`o" + pData->displayName + " `ochanged the price of `2" + getItemDef(world->items.at(x + (y * world->width)).vid).name + "`o to `5" + to_string(setprice) + " World Locks each.`7]");
														Player::OnTalkBubble(currentPeer, pData->netID, "`7[`w" + pData->displayName + " `wchanged the price of `2" + getItemDef(world->items.at(x + (y * world->width)).vid).name + "`w to `5" + to_string(setprice) + " World Locks each.`7]", 0, true);
													}
													Player::OnParticleEffect(currentPeer, 44, x * 32, y * 32, 0);
													if (world->items.at(x + (y * world->width)).opened && world->items.at(x + (y * world->width)).vcount < world->items.at(x + (y * world->width)).vprice) {
														UpdateVend(currentPeer, x, y, 0, islocks, setprice, world->items.at(x + (y * world->width)).background, world->items.at(x + (y * world->width)).opened);
													}
													else UpdateVend(currentPeer, x, y, world->items.at(x + (y * world->width)).vid, islocks, setprice, world->items.at(x + (y * world->width)).background, world->items.at(x + (y * world->width)).opened);
												}
											}
											break;
										}
										/*Buyerio Puse*/
										if (infoDat.at(0) == "buycount") {
											string buycountstr = infoDat.at(1);
											if (buycountstr.size() > 3 || buycountstr.size() <= 0) break;
											buyprice = atoi(buycountstr.c_str());
											if (buyprice <= 0) break;
											int x = pData->lastPunchX;
											int y = pData->lastPunchY;
											if (world->items.at(x + (y * world->width)).foreground != 2978) break;
											auto realprice = world->items.at(x + (y * world->width)).vprice;
											auto kiekmoketi = buyprice * realprice;
											auto worldlocks = 0;
											auto diamondlocks = 0;
											auto bgl = 0;
											auto locks = 0;
											for (auto i = 0; i < pData->inventory.items.size(); i++) {
												if (pData->inventory.items.at(i).itemID == 242) {
													worldlocks = pData->inventory.items.at(i).itemCount;
												}
												if (pData->inventory.items.at(i).itemID == 1796) {
													diamondlocks = pData->inventory.items.at(i).itemCount;
												}
												if (pData->inventory.items.at(i).itemID == 7188) {
													bgl = pData->inventory.items.at(i).itemCount;
												}
											}
											if (diamondlocks > 0) locks = diamondlocks * 100;
											locks += worldlocks;
											if (kiekmoketi > locks && !world->items.at(x + (y * world->width)).opened) {
												Player::OnTalkBubble(peer, pData->netID, "You can't afford that many!", 0, true);
												break;
											}
											if (bgl > 0) locks = bgl * 100;
											locks += diamondlocks;
											if (kiekmoketi > locks && !world->items.at(x + (y * world->width)).opened) {
												Player::OnTalkBubble(peer, pData->netID, "You can't afford that many!", 0, true);
												break;
											}
											if (CheckItemMaxed(peer, world->items.at(x + (y * world->width)).vid, buyprice)) {
												Player::OnTalkBubble(peer, pData->netID, "That wouldnt fit into my inventory!", 0, true);
												break;
											}
											int yracount = world->items.at(x + (y * world->width)).vcount;
											int yrawithdraw = world->items.at(x + (y * world->width)).vdraw;
											if (yracount < buyprice) {
												Player::OnTalkBubble(peer, pData->netID, "Out of order", 0, true);
												break;
											}
											if (world->items.at(x + (y * world->width)).opened) {
												if (buyprice < world->items.at(x + (y * world->width)).vprice) {
													Player::OnConsoleMessage(peer, "You have to buy at least 1 World Lock's worth.");
													Player::OnTalkBubble(peer, pData->netID, "You have to buy at least 1 World Lock's worth.", 0, true);
													break;
												}
												int oldbuyprice = buyprice;
												buyprice = round(((double)buyprice / world->items.at(x + (y * world->width)).vprice));
												kiekmoketi = buyprice;
												buyprice *= world->items.at(x + (y * world->width)).vprice;
												if (buyprice > world->items.at(x + (y * world->width)).vcount) {
													buyprice = round(oldbuyprice / world->items.at(x + (y * world->width)).vprice);
													kiekmoketi = buyprice;
													buyprice *= world->items.at(x + (y * world->width)).vprice;
												}
											}
											if (kiekmoketi > locks) {
												Player::OnTalkBubble(peer, pData->netID, "You can't afford that many!", 0, true);
												break;
											}
											pData->lastvendbuycount = to_string(buyprice);
											Player::OnDialogRequest(peer, "set_default_color|`o\nadd_label_with_icon|big|`wPurchase Confirmation``|left|1366|\nadd_spacer|small|\nadd_spacer|small|\nadd_textbox|`4You'll give:``|left|\nadd_spacer|small|\nadd_label_with_icon|small|(`w" + to_string(kiekmoketi) + "``) `8World Locks``|left|242|\nadd_spacer|small|\nadd_textbox|`2You'll get:``|left|\nadd_spacer|small|\nadd_label_with_icon|small|(`w" + to_string(buyprice) + "``) `2" + getItemDef(world->items.at(x + (y * world->width)).vid).name + "``|left|" + to_string(world->items.at(x + (y * world->width)).vid) + "|\nadd_spacer|small|\nadd_spacer|small|\nadd_textbox|Are you sure you want to make this purchase?|left|\nend_dialog|vending_buy|Cancel|OK|");
											break;
										}
									}
									if (isDropDialog)
									{
										if (world == nullptr || pData->currentWorld == "EXIT" || serverIsFrozen) break;
										if (infoDat.at(0) == "dropitemcount") {
											if (pData->trade) end_trade(peer);
											dropitemcount = infoDat.at(1);
											if (dropitemcount.size() > 3 || dropitemcount.size() <= 0) break;
											if (world->DisableDrop && pData->rawName != world->owner && !mod(peer)) {
												Player::OnTextOverlay(peer, "Dropping is disabled here!");
												break;
											}
											auto x = atoi(dropitemcount.c_str());
											short int currentItemCount = 0;
											for (auto i = 0; i < pData->inventory.items.size(); i++) {
												if (pData->inventory.items.at(i).itemID == pData->lastdropitem) {
													currentItemCount = static_cast<unsigned int>(pData->inventory.items.at(i).itemCount);
													break;
												}
											}
											if (x <= 0 || x > currentItemCount) break;
											auto iscontainseas = false;
											SearchInventoryItem(peer, pData->lastdropitem, 1, iscontainseas);
											if (!iscontainseas) break;
											if (pData->lastdropitem == 6260 || getItemDef(pData->lastdropitem).properties & Property_Untradable || pData->lastdropitem == 18 || pData->lastdropitem == 32 || pData->lastdropitem == 6336 || pData->lastdropitem == 1794 || pData->lastdropitem == 5640 || pData->lastdropitem == 1424 || pData->lastdropitem == 6260) {
												if (!admin(peer) && !pData->Subscriber || pData->lastdropitem == 18 || pData->lastdropitem == 5640 || pData->lastdropitem == 32 || pData->lastdropitem == 6336 || pData->lastdropitem == 1486 || pData->lastdropitem == 1794 || pData->lastdropitem == 1424 || pData->lastdropitem == 6260) {
													Player::OnTextOverlay(peer, "You can't drop that.");
													break;
												}
											}
											if (getItemDef(pData->lastdropitem).blockType == BlockTypes::FISH && x != currentItemCount) break;
											if (!isWorldOwner(peer, world) && !isWorldAdmin(peer, world) && mod(peer) && world->owner != "") {
											}
											auto xx = pData->x + (32 * (pData->isRotatedLeft ? -1 : 1));
											auto yy = pData->y;
											RemoveInventoryItem(pData->lastdropitem, x, peer, true);
											updateplayerset(peer, pData->lastdropitem);
											if (world->name == "GROWGANOTH" && GrowganothEvent && yy / 32 == 15 && pData->x / 32 >= 48 && pData->x / 32 <= 51) {
												SendSacrifice(world, peer, pData->lastdropitem, x);
												continue;
											}
											if (world->name != "EXIT" && pData->isIn) {
												DropItem(world, peer, -1, xx, yy + (rand() % 16), pData->lastdropitem, x, 0);
											}
											pData->lastdropitem = 0;
										}
									}
									if (isTrashDialog || trashitem2) {
										if (world == nullptr || pData->currentWorld == "EXIT" || serverIsFrozen) break;
										if (infoDat.at(0) == "trashitemcount" || trashitem2) {
											if (pData->trade) end_trade(peer);
											if (!trashitem2) {
												trashitemcount = infoDat.at(1);
											}
											else {
												trashitemcount = to_string(pData->lasttrashitemcount);
											}
											if (trashitemcount.size() > 3 || trashitemcount.size() <= 0 || pData->lasttrashitem == 0) break;
											if (getItemDef(pData->lasttrashitem).properties & Property_Untradable && pData->lasttrashitemcount == 0) {
												pData->lasttrashitemcount = atoi(trashitemcount.c_str());
												Player::OnDialogRequest(peer, "set_default_color|`o\nadd_label_with_icon|big|`4Recycle`` " + trashitemcount + " `w" + getItemDef(pData->lasttrashitem).name + "``|left|" + to_string(pData->lasttrashitem) + "|\nadd_textbox|You are recycling an `9UNTRADEABLE`` item. Are you absolutely sure you want to do this? There is no way to get the item back if you select yes.|left|\nend_dialog|trash_item2|NO!|Yes, I am sure|");
												break;
											}
											auto gemtrashcount = 0;
											auto x = atoi(trashitemcount.c_str());
											short int currentItemCount = 0;
											for (auto i = 0; i < pData->inventory.items.size(); i++) {
												if (pData->inventory.items.at(i).itemID == pData->lasttrashitem) {
													currentItemCount = static_cast<unsigned int>(pData->inventory.items.at(i).itemCount);
													break;
												}
											}
											if (x <= 0 || x > currentItemCount) break;
											if (!admin(peer)) {
												auto iscontainseas = false;
												SearchInventoryItem(peer, pData->lasttrashitem, 1, iscontainseas);
												if (!iscontainseas) break;
											}
											if (getItemDef(pData->lasttrashitem).blockType == BlockTypes::FISH && x != currentItemCount) break;
											if (getItemDef(pData->lasttrashitem).blockType == BlockTypes::FISH) {
												gemtrashcount = (rand() % 4 + 1) * x;
											}
											Player::OnConsoleMessage(peer, "`w" + std::to_string(x) + " " + getItemDef(pData->lasttrashitem).name + " `orecycled, `w" + std::to_string(gemtrashcount) + " `ogems earned.");
											RemoveInventoryItem(pData->lasttrashitem, x, peer, true);
											sendSound(peer, "trash.wav");
											updateplayerset(peer, pData->lasttrashitem);
											std::ifstream ifsz("save/gemdb/_" + pData->rawName + ".txt");
											std::string content((std::istreambuf_iterator<char>(ifsz)), (std::istreambuf_iterator<char>()));
											auto gembux = atoi(content.c_str());
											auto fingembux = gembux + gemtrashcount;
											ofstream myfile;
											myfile.open("save/gemdb/_" + pData->rawName + ".txt");
											myfile << fingembux;
											myfile.close();
											int gemcalc = gembux + gemtrashcount;
											Player::OnSetBux(peer, gemcalc, 0);
											pData->lasttrashitem = 0;
											pData->lasttrashitemcount = 0;
										}
									}
									if (issafewithdrawConfirmDialog) {
										if (infoDat.at(0).substr(0, 13) == "safewithdraw_") {
											safe_withdrawConfirmCount = infoDat.at(1);
											if (safe_withdrawConfirmCount.size() > 5) break;
											string coord_and_apos = infoDat.at(0).erase(0, 13);
											string::size_type pos = coord_and_apos.find('_');
											if (pos != std::string::npos) {
												safe_withdrawConfirmCoord = coord_and_apos.substr(0, pos);
												coord_and_apos.erase(0, pos + 1);
												safe_withdrawConfirmApos = coord_and_apos;
											}
											else break;
											auto isdbox = std::experimental::filesystem::exists("save/safevault/_" + pData->currentWorld + "/X" + safe_withdrawConfirmCoord + ".json");
											if (!isdbox) break;
											bool contains_non_int3 = !std::regex_match(safe_withdrawConfirmApos, std::regex("^[0-9]+$"));
											if (contains_non_int3 == true) break;
											bool contains_non_int4 = !std::regex_match(safe_withdrawConfirmCount, std::regex("^[0-9]+$"));
											if (contains_non_int4 == true) {
												Player::OnTalkBubble(peer, pData->netID, "Invalid input in amount!", 0, false);
												break;
											}
											safe_withdrawConfirmCountInt = atoi(safe_withdrawConfirmCount.c_str());
											safe_withdrawConfirmAposInt = atoi(safe_withdrawConfirmApos.c_str());
											if (safe_withdrawConfirmCountInt < 1 || safe_withdrawConfirmCountInt > 250) {
												Player::OnTalkBubble(peer, pData->netID, "Invalid input in amount!", 0, false);
												break;
											}

											ifstream ifff("save/safevault/_" + pData->currentWorld + "/X" + safe_withdrawConfirmCoord + ".json");
											json j;
											ifff >> j;
											ifff.close();
											int itemid = 0;
											bool success = true;
											for (int i = 0; i < 20; i++)
											{
												if (j["safe"].at(i)["aposition"] == safe_withdrawConfirmAposInt)
												{
													itemid = j["safe"].at(i)["itemid"];

													if (itemid == 0) break;

													if (safe_withdrawConfirmCountInt > j["safe"].at(i)["itemcount"])
													{
														Player::OnTalkBubble(peer, pData->netID, "You don't have that many stored!", 0, true);
														break;
													}

													if (CheckItemExists(peer, j["safe"].at(i)["itemid"]))
													{
														int currentlyHave = GetQuantityOfItem(peer, j["safe"].at(i)["itemid"]);
														if (safe_withdrawConfirmCountInt + currentlyHave < 251)
														{
															SaveItemMoreTimes(j["safe"].at(i)["itemid"], safe_withdrawConfirmCountInt, peer, success, pData->rawName + " withdraw from safe vault");
														}
														else
														{
															Player::OnTextOverlay(peer, "`4You don't have enought space.");
															break;
														}
													}
													else
													{
														if (pData->inventory.items.size() == pData->currentInventorySize)
														{
															Player::OnTextOverlay(peer, "`4Your inventory is full! You can purchase an inventory upgrade in the shop.");
															break;
														}

														SaveItemMoreTimes(j["safe"].at(i)["itemid"], safe_withdrawConfirmCountInt, peer, success, pData->rawName + " withdraw from safe vault");
													}

													Player::OnTalkBubble(peer, pData->netID, "`wRemoved " + to_string(safe_withdrawConfirmCountInt) + " " + itemDefs[itemid].name + " from the Safe Vault", 0, true);

													if (j["safe"].at(i)["itemcount"] == safe_withdrawConfirmCountInt)
													{
														j["safe"].at(i)["placedby"] = "There was " + to_string(safe_withdrawConfirmCountInt) + " of " + to_string(itemid) + " item, and " + pData->rawName + " withdrawed them.";
														j["safe"].at(i)["itemid"] = 0;
														j["safe"].at(i)["itemcount"] = 0;
														j["insafe"] = j["insafe"].get<double>() - 1;
													}
													else
													{
														j["safe"].at(i)["itemcount"] = j["safe"].at(i)["itemcount"].get<double>() - safe_withdrawConfirmCountInt;
													}

													ofstream of("save/safevault/_" + pData->currentWorld + "/X" + safe_withdrawConfirmCoord + ".json");
													of << j << std::endl;
													of.close();
													break;
												}
											}
										}
									}
									if (isboxlvl2AddItemDialog)
									{
										if (infoDat.at(0).substr(0, 15) == "boxlvl2deposit_") boxlvl2AddItemDialog = infoDat.at(1);
										if (boxlvl1AddItemDialog.size() > 5) break;
										string coord = infoDat.at(0).substr(15, infoDat.at(0).length() - 1).c_str();
										int itemid = atoi(boxlvl2AddItemDialog.c_str());

										if (itemid < 1 || itemid > maxItems)
										{
											break;
										}
										if (itemid == 18 || itemid == 32 || itemid == 6336 || getItemDef(itemid).properties & Property_Untradable || getItemDef(itemid).blockType == BlockTypes::FISH || itemid == 8552 || itemid == 9472 || itemid == 9482 || itemid == 9356 || itemid == 9492 || itemid == 9498 || itemid == 8774 || itemid == 1790 || itemid == 2592 || itemid == 1784 || itemid == 1792 || itemid == 1794 || itemid == 7734 || itemid == 8306 || itemid == 9458)
										{
											Player::OnTextOverlay(peer, "`4You can't place " + getItemDef(itemid).name + ".");
											break;
										}

										auto issafe = std::experimental::filesystem::exists("save/storageboxlvl2/_" + pData->currentWorld + "/X" + coord + ".json");
										if (!issafe)
										{
											Player::OnTalkBubble(peer, pData->netID, "`4An error occured while getting Storage Box Xtreme - Level 2 info", 0, true);
											break;
										}

										int currentlyHave = 0;
										currentlyHave = GetQuantityOfItem(peer, itemid);

										auto contains = false;
										SearchInventoryItem(peer, itemid, currentlyHave, contains);
										if (!contains)
										{
											break;
										}

										ifstream ifff("save/storageboxlvl2/_" + pData->currentWorld + "/X" + coord + ".json");
										json j;
										ifff >> j;
										ifff.close();

										if (j["instorage"] > 19)
										{
											Player::OnTalkBubble(peer, pData->netID, "`4Storage Box Xtreme - Level 2 is full!", 0, true);
											break;
										}

										GTDialog mysafe;
										mysafe.addLabelWithIcon("`wStorage Box Xtreme - Level 2 info", 6288, LABEL_BIG);
										mysafe.addSmallText("You have " + to_string(currentlyHave) + " " + getItemDef(itemid).name + ". How many to store?");
										mysafe.addInputBox("boxlvl2depositcount_" + coord + "_" + to_string(itemid) + "", "", "", 5);
										mysafe.addSpacer(SPACER_SMALL);
										mysafe.addButton("boxlvl2depositconfirm", "Store items");
										mysafe.addSpacer(SPACER_SMALL);
										mysafe.addQuickExit();
										mysafe.endDialog("Close", "", "Exit");
										Player::OnDialogRequest(peer, mysafe.finishDialog());
									}
									if (isboxlvl3AddItemDialog)
									{
										if (infoDat.at(0).substr(0, 15) == "boxlvl3deposit_") boxlvl3AddItemDialog = infoDat.at(1);
										if (boxlvl1AddItemDialog.size() > 5) break;
										string coord = infoDat.at(0).substr(15, infoDat.at(0).length() - 1).c_str();
										int itemid = atoi(boxlvl3AddItemDialog.c_str());

										if (itemid < 1 || itemid > maxItems)
										{
											break;
										}
										if (itemid == 18 || itemid == 32 || itemid == 6336 || getItemDef(itemid).properties & Property_Untradable || getItemDef(itemid).blockType == BlockTypes::FISH || itemid == 8552 || itemid == 9472 || itemid == 9482 || itemid == 9356 || itemid == 9492 || itemid == 9498 || itemid == 8774 || itemid == 1790 || itemid == 2592 || itemid == 1784 || itemid == 1792 || itemid == 1794 || itemid == 7734 || itemid == 8306 || itemid == 9458)
										{
											Player::OnTextOverlay(peer, "`4You can't place " + getItemDef(itemid).name + ".");
											break;
										}

										auto issafe = std::experimental::filesystem::exists("save/storageboxlvl3/_" + pData->currentWorld + "/X" + coord + ".json");
										if (!issafe)
										{
											Player::OnTalkBubble(peer, pData->netID, "`4An error occured while getting Storage Box Xtreme - Level 3 info", 0, true);
											break;
										}

										int currentlyHave = 0;
										currentlyHave = GetQuantityOfItem(peer, itemid);

										auto contains = false;
										SearchInventoryItem(peer, itemid, currentlyHave, contains);
										if (!contains)
										{
											break;
										}

										ifstream ifff("save/storageboxlvl3/_" + pData->currentWorld + "/X" + coord + ".json");
										json j;
										ifff >> j;
										ifff.close();

										if (j["instorage"] > 19)
										{
											Player::OnTalkBubble(peer, pData->netID, "`4Storage Box Xtreme - Level 3 is full!", 0, true);
											break;
										}

										GTDialog mysafe;
										mysafe.addLabelWithIcon("`wStorage Box Xtreme - Level 3 info", 6290, LABEL_BIG);
										mysafe.addSmallText("You have " + to_string(currentlyHave) + " " + getItemDef(itemid).name + ". How many to store?");
										mysafe.addInputBox("boxlvl3depositcount_" + coord + "_" + to_string(itemid) + "", "", "", 5);
										mysafe.addSpacer(SPACER_SMALL);
										mysafe.addButton("boxlvl3depositconfirm", "Store items");
										mysafe.addSpacer(SPACER_SMALL);
										mysafe.addQuickExit();
										mysafe.endDialog("Close", "", "Exit");
										Player::OnDialogRequest(peer, mysafe.finishDialog());
									}
									if (isboxlvl1AddItemDialog)
									{
										if (infoDat.at(0).substr(0, 15) == "boxlvl1deposit_") boxlvl1AddItemDialog = infoDat.at(1);
										if (boxlvl1AddItemDialog.size() > 5) break;
										string coord = infoDat.at(0).substr(15, infoDat.at(0).length() - 1).c_str();
										int itemid = atoi(boxlvl1AddItemDialog.c_str());

										if (itemid < 1 || itemid > maxItems)
										{
											break;
										}
										if (itemid == 18 || itemid == 32 || itemid == 6336 || getItemDef(itemid).properties & Property_Untradable || getItemDef(itemid).blockType == BlockTypes::FISH || itemid == 8552 || itemid == 9472 || itemid == 9482 || itemid == 9356 || itemid == 9492 || itemid == 9498 || itemid == 8774 || itemid == 1790 || itemid == 2592 || itemid == 1784 || itemid == 1792 || itemid == 1794 || itemid == 7734 || itemid == 8306 || itemid == 9458)
										{
											Player::OnTextOverlay(peer, "`4You can't place " + getItemDef(itemid).name + ".");
											break;
										}

										auto issafe = std::experimental::filesystem::exists("save/storageboxlvl1/_" + pData->currentWorld + "/X" + coord + ".json");
										if (!issafe)
										{
											Player::OnTalkBubble(peer, pData->netID, "`4An error occured while getting Storage Box Xtreme - Level 1 info", 0, true);
											break;
										}

										int currentlyHave = 0;
										currentlyHave = GetQuantityOfItem(peer, itemid);

										auto contains = false;
										SearchInventoryItem(peer, itemid, currentlyHave, contains);
										if (!contains)
										{
											break;
										}

										ifstream ifff("save/storageboxlvl1/_" + pData->currentWorld + "/X" + coord + ".json");
										json j;
										ifff >> j;
										ifff.close();

										if (j["instorage"] > 19)
										{
											Player::OnTalkBubble(peer, pData->netID, "`4Storage Box Xtreme - Level 1 is full!", 0, true);
											break;
										}

										GTDialog mysafe;
										mysafe.addLabelWithIcon("`wStorage Box Xtreme - Level 1 info", 6286, LABEL_BIG);
										mysafe.addSmallText("You have " + to_string(currentlyHave) + " " + getItemDef(itemid).name + ". How many to store?");
										mysafe.addInputBox("boxlvl1depositcount_" + coord + "_" + to_string(itemid) + "", "", "", 5);
										mysafe.addSpacer(SPACER_SMALL);
										mysafe.addButton("boxlvl1depositconfirm", "Store items");
										mysafe.addSpacer(SPACER_SMALL);
										mysafe.addQuickExit();
										mysafe.endDialog("Close", "", "Exit");
										Player::OnDialogRequest(peer, mysafe.finishDialog());
									}
									if (isSafeAddItemDialog)
									{
										if (infoDat.at(0).substr(0, 12) == "safedeposit_") SafeAddItemDialog = infoDat.at(1);
										if (SafeAddItemDialog.size() > 5) break;
										string coord = infoDat.at(0).substr(12, infoDat.at(0).length() - 1).c_str();
										int itemid = atoi(SafeAddItemDialog.c_str());

										if (itemid < 1 || itemid > maxItems)
										{
											break;
										}
										if (itemid == 18 || itemid == 32 || itemid == 6336 || getItemDef(itemid).properties & Property_Untradable || getItemDef(itemid).blockType == BlockTypes::FISH || itemid == 8552 || itemid == 9472 || itemid == 9482 || itemid == 9356 || itemid == 9492 || itemid == 9498 || itemid == 8774 || itemid == 1790 || itemid == 2592 || itemid == 1784 || itemid == 1792 || itemid == 1794 || itemid == 7734 || itemid == 8306 || itemid == 9458)
										{
											Player::OnTextOverlay(peer, "`4You can't place " + getItemDef(itemid).name + ".");
											break;
										}

										auto issafe = std::experimental::filesystem::exists("save/safevault/_" + pData->currentWorld + "/X" + coord + ".json");
										if (!issafe)
										{
											Player::OnTalkBubble(peer, pData->netID, "`4An error occured while getting donation box info.", 0, true);
											break;
										}

										int currentlyHave = 0;
										currentlyHave = GetQuantityOfItem(peer, itemid);

										auto contains = false;
										SearchInventoryItem(peer, itemid, currentlyHave, contains);
										if (!contains)
										{
											break;
										}

										ifstream ifff("save/safevault/_" + pData->currentWorld + "/X" + coord + ".json");
										json j;
										ifff >> j;
										ifff.close();

										if (j["insafe"] > 19)
										{
											Player::OnTalkBubble(peer, pData->netID, "`4Safe Vault is full!", 0, true);
											break;
										}

										GTDialog mysafe;
										mysafe.addLabelWithIcon("`wSafe fault", 8878, LABEL_BIG);
										mysafe.addSmallText("You have " + to_string(currentlyHave) + " " + getItemDef(itemid).name + ". How many to store?");
										mysafe.addInputBox("safedepositcount_" + coord + "_" + to_string(itemid) + "", "", "", 5);
										mysafe.addSpacer(SPACER_SMALL);
										mysafe.addQuickExit();
										mysafe.endDialog("safedepositconfirm", "Store items", "Cancel");
										Player::OnDialogRequest(peer, mysafe.finishDialog());
									}
									if (issafedepositconfirmDialog) {
										if (infoDat.at(0).substr(0, 17) == "safedepositcount_") {
											safedepositcount = infoDat.at(1);
											string coord_and_id = infoDat.at(0).erase(0, 17);
											string::size_type pos = coord_and_id.find('_');
											if (pos != std::string::npos) {
												safedepositcoord = coord_and_id.substr(0, pos);
												coord_and_id.erase(0, pos + 1);
												safedeposititemID = coord_and_id;
											}
											else break;
											auto isdbox = std::experimental::filesystem::exists("save/safevault/_" + pData->currentWorld + "/X" + safedepositcoord + ".json");
											if (!isdbox) break;
											bool contains_non_int3 = !std::regex_match(safedeposititemID, std::regex("^[0-9]+$"));
											if (contains_non_int3 == true) break;
											bool contains_non_int4 = !std::regex_match(safedepositcount, std::regex("^[0-9]+$"));
											if (contains_non_int4 == true) {
												Player::OnTalkBubble(peer, pData->netID, "Invalid input in amount!", 0, false);
												break;
											}
											givesafeitemCountInt = atoi(safedepositcount.c_str());
											givesafeitemItemIDInt = atoi(safedeposititemID.c_str());
											if (givesafeitemItemIDInt < 1 || givesafeitemItemIDInt > maxItems) break;
											if (givesafeitemCountInt < 1 || givesafeitemCountInt > 250) {
												Player::OnTalkBubble(peer, pData->netID, "Invalid input in amount!", 0, false);
												break;
											}

											ifstream ifff("save/safevault/_" + pData->currentWorld + "/X" + safedepositcoord + ".json");
											json j;
											ifff >> j;
											ifff.close();
											if (j["insafe"] > 19) {
												Player::OnTalkBubble(peer, pData->netID, "`4Safe fault is full!", 0, true);
												break;
											}
											bool isContains = false;
											SearchInventoryItem(peer, givesafeitemItemIDInt, givesafeitemCountInt, isContains);
											if (isContains == false) {
												Player::OnTextOverlay(peer, "`4You don't have enough " + itemDefs[givesafeitemItemIDInt].name + " in your inventory.");
												break;
											}
											j["insafe"] = j["insafe"].get<double>() + 1;
											for (int i = 0; i < 20; i++) {
												if (j["safe"].at(i)["itemid"] == 0) {
													j["safe"].at(i)["itemid"] = givesafeitemItemIDInt;
													j["safe"].at(i)["itemcount"] = givesafeitemCountInt;
													j["safe"].at(i)["placedby"] = pData->rawName;
													Player::OnTalkBubble(peer, pData->netID, "`wStored " + to_string(givesafeitemCountInt) + " " + itemDefs[givesafeitemItemIDInt].name + " `win the Safe Vault", 0, true);
													break;
												}
											}
											ofstream of("save/safevault/_" + pData->currentWorld + "/X" + safedepositcoord + ".json");
											of << j << std::endl;
											of.close();
											RemoveInventoryItem(givesafeitemItemIDInt, givesafeitemCountInt, peer, true);
											updateplayerset(peer, givesafeitemItemIDInt);
										}
									}
									if (signEditor) {
										if (world == nullptr || pData->currentWorld == "EXIT" || serverIsFrozen) continue;
										if (infoDat.at(0) == "signtextas") message321 = infoDat.at(1);
									}
									if (isEntranceDialog) {
										if (infoDat.at(0) == "checkbox_public") {
											entranceresult = infoDat.at(1);
											if (entranceresult != "0" && entranceresult != "1") break;
											if (getItemDef(world->items.at(pData->wrenchx + (pData->wrenchy * world->width)).foreground).blockType != BlockTypes::GATEWAY) break;
											if (isWorldOwner(peer, world) || isWorldAdmin(peer, world) || mod(peer) || !restricted_area(peer, world, pData->wrenchx, pData->wrenchy)) {
												world->items.at(pData->wrenchx + (pData->wrenchy * world->width)).opened = entranceresult == "0" ? 0 : 1;
												for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
													if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
													if (isHere(peer, currentPeer)) {
														if (admin(currentPeer) || isWorldOwner(currentPeer, world) || isWorldAdmin(currentPeer, world)) {
															update_entrance(currentPeer, world->items.at(pData->wrenchx + (pData->wrenchy * world->width)).foreground, pData->wrenchx, pData->wrenchy, true, world->items.at(pData->wrenchx + (pData->wrenchy * world->width)).background);
														}
														else {
															update_entrance(currentPeer, world->items.at(pData->wrenchx + (pData->wrenchy * world->width)).foreground, pData->wrenchx, pData->wrenchy, world->items.at(pData->wrenchx + (pData->wrenchy * world->width)).opened, world->items.at(pData->wrenchx + (pData->wrenchy * world->width)).background);
														}
													}
												}
												break;
											}
										}
									}
									if (MagUpdateDialog) {
										if (world == nullptr || pData->currentWorld == "EXIT" || serverIsFrozen) continue;
										if (infoDat.at(0) == "checkbox_publicremote") {
											publicremote = infoDat.at(1);
											if (publicremote != "1" && publicremote != "0") break;
											if (isWorldOwner(peer, world) || mod(peer)) {
												int x = pData->lastPunchX;
												int y = pData->lastPunchY;
												if (publicremote == "1") world->items.at(x + (y * world->width)).rm = true;
												else if (publicremote == "0") world->items.at(x + (y * world->width)).rm = false;
												Player::OnTalkBubble(peer, pData->netID, "Finished!", 0, true);
												break;
											}
										}
										if (infoDat.at(0) == "checkbox_gemcollection") {
											if (infoDat.at(1) == "1") {
												int x = pData->lastPunchX;
												int y = pData->lastPunchY;
												world->items.at(x + (y * world->width)).mid = 112;
												ENetPeer* currentPeer;
												for (currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
													if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
													if (isHere(peer, currentPeer)) {
														send_item_sucker(currentPeer, 5638, x, y, 112, 0, true, true, world->items.at(x + (y * world->width)).background);
													}
												}
											}
											else {
												int x = pData->lastPunchX;
												int y = pData->lastPunchY;
												world->items.at(x + (y * world->width)).mid = 0;
												ENetPeer* currentPeer;
												for (currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
													if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
													if (isHere(peer, currentPeer)) {
														send_item_sucker(currentPeer, 5638, x, y, 0, 0, true, true, world->items.at(x + (y * world->width)).background);
													}
												}
											}
											break;
										}
									}
									if (isgivedonationboxitemConfirm)
									{
										if (world == nullptr || pData->currentWorld == "EXIT" || serverIsFrozen) break;
										if (infoDat.at(0).substr(0, 25) == "givedonationboxitemCount_")
										{
											givedonationboxitemCount = infoDat.at(1);
											string coord_and_id = infoDat.at(0).erase(0, 25);
											string::size_type pos = coord_and_id.find('_');
											if (pos != std::string::npos)
											{
												givedonationboxitemCoord = coord_and_id.substr(0, pos);
												coord_and_id.erase(0, pos + 1);
												givedonationboxitemItemID = coord_and_id;
											}
											else
											{
												break;
											}
											auto isdbox = std::experimental::filesystem::exists("save/donationboxes/_" + pData->currentWorld + "/X" + givedonationboxitemCoord + ".json");
											if (!isdbox)
											{
												Player::OnTalkBubble(peer, pData->netID, "`4An error occured while getting donation box info.", 0, true);
												break;
											}
											bool contains_non_int3 = !std::regex_match(givedonationboxitemItemID, std::regex("^[0-9]+$"));
											if (contains_non_int3 == true)
											{
												break;
											}
											bool contains_non_int4 = !std::regex_match(givedonationboxitemCount, std::regex("^[0-9]+$"));
											if (contains_non_int4 == true)
											{
												Player::OnTalkBubble(peer, pData->netID, "Invalid input in amount!", 0, false);
												break;
											}
											givedonationboxitemCountInt = atoi(givedonationboxitemCount.c_str());
											givedonationboxitemItemIDInt = atoi(givedonationboxitemItemID.c_str());
											if (givedonationboxitemItemIDInt < 1 || givedonationboxitemItemIDInt > maxItems)
											{
												break;
											}

											if (getItemDef(givedonationboxitemItemIDInt).rarity < 2)
											{
												Player::OnTalkBubble(peer, pData->netID, "`4You can donate items higher than 2 rarity!", 0, true);
												break;
											}
											if (givedonationboxitemCountInt < 1 || givedonationboxitemCountInt > 250)
											{
												Player::OnTalkBubble(peer, pData->netID, "Invalid input in amount!", 0, false);
												break;
											}
										}
										if (infoDat.at(0) == "givedonationboxitemNote")
										{
											givedonationboxitemNote = infoDat.at(1);
											if (givedonationboxitemNote.size() < 1 || givedonationboxitemNote.size() > 50)
											{
												Player::OnTextOverlay(peer, "`4The note should be at least 1 letter.");
												break;
											}

											ifstream ifff("save/donationboxes/_" + pData->currentWorld + "/X" + givedonationboxitemCoord + ".json");
											json j;
											ifff >> j;
											ifff.close();
											if (j["donated"] > 19) {
												Player::OnTalkBubble(peer, pData->netID, "`4Donation box is full!", 0, true);
												break;
											}
											bool isContains = false;
											SearchInventoryItem(peer, givedonationboxitemItemIDInt, givedonationboxitemCountInt, isContains);
											if (isContains == false) {
												break;
											}
											RemoveInventoryItem(givedonationboxitemItemIDInt, givedonationboxitemCountInt, peer, true);
											updateplayerset(peer, givedonationboxitemItemIDInt);
											j["donated"] = j["donated"].get<double>() + 1;
											for (int i = 0; i < 20; i++) {
												if (j["donatedItems"].at(i)["itemid"] == 0) {
													j["donatedItems"].at(i)["itemid"] = givedonationboxitemItemIDInt;
													j["donatedItems"].at(i)["itemcount"] = givedonationboxitemCountInt;
													j["donatedItems"].at(i)["sentBy"] = pData->displayName;
													j["donatedItems"].at(i)["note"] = givedonationboxitemNote;
													for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
														if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
														if (isHere(peer, currentPeer)) {
															Player::OnTalkBubble(peer, pData->netID, "`w[`5[`w" + pData->displayName + " `wplaces `5" + to_string(givedonationboxitemCountInt) + " `2" + getItemDef(givedonationboxitemItemIDInt).name + " `winto the " + getItemDef(world->items.at(pData->lastPunchX + (pData->lastPunchY * world->width)).foreground).name + "`5]`w]", 0, true);
															Player::OnConsoleMessage(currentPeer, "`w[`5[`w" + pData->displayName + " `wplaces `5" + to_string(givedonationboxitemCountInt) + " `2" + getItemDef(givedonationboxitemItemIDInt).name + " `winto the " + getItemDef(world->items.at(pData->lastPunchX + (pData->lastPunchY * world->width)).foreground).name + "`5]`w]");
														}
													}
													break;
												}
											}
											ofstream of("save/donationboxes/_" + pData->currentWorld + "/X" + givedonationboxitemCoord + ".json");
											of << j << std::endl;
											of.close();
										}
									}
									if (isaddItemToDonationBox)
									{
										if (world == nullptr || pData->currentWorld == "EXIT" || serverIsFrozen) break;
										if (infoDat.at(0).substr(0, 16) == "addDonationItem_") addItemToDonationBox = infoDat.at(1);
										if (addItemToDonationBox.size() > 5) break;
										string coord = infoDat.at(0).substr(16, infoDat.at(0).length() - 1).c_str();
										int itemid = atoi(addItemToDonationBox.c_str());

										if (itemid < 1 || itemid > maxItems)
										{
											break;
										}
										if (itemid == 18 || itemid == 32 || itemid == 6336 || getItemDef(itemid).properties & Property_Untradable || getItemDef(itemid).blockType == BlockTypes::FISH || itemid == 8552 || itemid == 9472 || itemid == 9482 || itemid == 9356 || itemid == 9492 || itemid == 9498 || itemid == 8774 || itemid == 1790 || itemid == 2592 || itemid == 1784 || itemid == 1792 || itemid == 1794 || itemid == 7734 || itemid == 8306 || itemid == 9458)
										{
											Player::OnTextOverlay(peer, "`4You can't donate " + getItemDef(itemid).name + ".");
											break;
										}

										if (getItemDef(itemid).rarity < 2)
										{
											Player::OnTalkBubble(peer, pData->netID, "`4You can donate items higher than 2 rarity!", 0, true);
											break;
										}

										auto isdbox = std::experimental::filesystem::exists("save/donationboxes/_" + pData->currentWorld + "/X" + coord + ".json");
										if (!isdbox)
										{
											Player::OnTalkBubble(peer, pData->netID, "`4An error occured while getting donation box info.", 0, true);
											break;
										}

										ifstream ifff("save/donationboxes/_" + pData->currentWorld + "/X" + coord + ".json");
										json j;
										ifff >> j;
										ifff.close();

										if (j["donated"] > 19)
										{
											Player::OnTalkBubble(peer, pData->netID, "`4Donation box is full!", 0, true);
											break;
										}

										GTDialog myDbox;
										myDbox.addLabelWithIcon(getItemDef(itemid).name, itemid, LABEL_BIG);
										myDbox.addSmallText("`$How many to put in the box as a gift? (Note: you will `4LOSE `$the items you give!)");
										myDbox.addInputBox("givedonationboxitemCount_" + coord + "_" + to_string(itemid) + "", "Count:", "", 5);
										myDbox.addSpacer(SPACER_SMALL);
										myDbox.addInputBox("givedonationboxitemNote", "Optional Note:", "", 50);

										myDbox.addSpacer(SPACER_SMALL);
										myDbox.addQuickExit();
										myDbox.endDialog("givedonationboxitemConfirm", "`4Give the item(s)", "Cancel");
										Player::OnDialogRequest(peer, myDbox.finishDialog());
									}
									if (isEditDoorDialog) {
										if (infoDat.at(0) == "door_target") {
											string a = getStrUpper(infoDat.at(1));
											vector<string> b = explode(":", a);
											if (b.size() == 1) {
												door_destination_world = b[0];
											}
											else if (b.size() > 1) {
												door_destination_world = b[0];
												door_destination_id = b[1];
												if (door_destination_world == "") {
													door_destination_world = pData->currentWorld;
												}
											}
										}
										if (infoDat.at(0) == "door_name") door_name = infoDat.at(1);
										if (infoDat.at(0) == "door_id") {
											door_id = getStrUpper(infoDat.at(1));
											world->items.at(pData->wrenchsession).destWorld = door_destination_world;
											world->items.at(pData->wrenchsession).destId = door_destination_id;
											world->items.at(pData->wrenchsession).label = door_name;
											if (world->items.at(pData->wrenchsession).foreground == 762) {
												world->items.at(pData->wrenchsession).password = door_id;
											}
											else {
												world->items.at(pData->wrenchsession).currId = door_id;
											}
											updateDoor(peer, world->items.at(pData->wrenchsession).foreground, pData->wrenchsession % world->width, pData->wrenchsession / world->width, world->items.at(pData->wrenchsession).background, door_name == "" ? (door_destination_id == "" ? door_destination_world : door_destination_world + "...") : door_name, false, false);
										}
									}
									if (title_save) {
										if (infoDat.at(0) == "title_name") {
											if (!pData->Subscriber) break;
											title_name = infoDat.at(1);
											if (title_name.size() > 10 || title_name.size() < 0) break;
											if (title_name.find("@") != string::npos) {
												Player::OnTalkBubble(peer, pData->netID, "`oSorry this title cannot be used for security reasons!", 0, true);
												break;
											}
											if (title_name.size() == 0) {
												Player::OnTalkBubble(peer, pData->netID, "`oTitle have been removed!", 0, true);
												pData->NickPrefix = "";
												restoreplayernick(peer);
												Player::OnNameChanged(peer, pData->netID, pData->displayName);
											}
											else {
												pData->NickPrefix = title_name;
												Player::OnTalkBubble(peer, pData->netID, "`oTitle changed to " + title_name + "!", 0, true);
												restoreplayernick(peer);
												Player::OnNameChanged(peer, pData->netID, pData->displayName);
											}
										}
									}
									if (notebook_edit) {
										if (infoDat.at(0) == "personal_note") {
											personal_note = infoDat.at(1);
											if (personal_note.size() >= 128 || personal_note.size() <= 0) break;
											pData->notebook = personal_note;
											Player::OnTalkBubble(peer, pData->netID, "`2Text saved.", 0, true);
										}
									}
									if (signEditor) {
										int x = pData->wrenchedBlockLocation % world->width;
										int y = pData->wrenchedBlockLocation / world->width;
										if (x < 0 || y < 0) break;
										world->items.at(pData->wrenchedBlockLocation).sign = message321;
										UpdateMessageVisuals(peer, world->items.at(pData->wrenchedBlockLocation).foreground, x, y, message321, world->items.at(pData->wrenchedBlockLocation).background);
									}
									if (isLockDialog) {
										if (infoDat.at(1) == "clear_mascot") {
											try {
												ifstream read_player("save/players/_" + world->owner + ".json");
												if (!read_player.is_open()) {
													break;
												}
												json j;
												read_player >> j;
												read_player.close();
												string guild_name = j["guild"];
												for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
													if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
													if (static_cast<PlayerInfo*>(currentPeer->data)->guild == guild_name) {
														static_cast<PlayerInfo*>(currentPeer->data)->guildFg = 0;
														static_cast<PlayerInfo*>(currentPeer->data)->guildBg = 0;
														Player::OnTalkBubble(currentPeer, static_cast<PlayerInfo*>(currentPeer->data)->netID, "Your guild mascot have been cleared", 0, true);
														Player::OnConsoleMessage(currentPeer, "`oYour guild mascot have been cleared");
														Player::PlayAudio(currentPeer, "audio/siren.wav", 0);
														for (ENetPeer* currentPeer2 = server->peers; currentPeer2 < &server->peers[server->peerCount]; ++currentPeer2) {
															if (currentPeer2->state != ENET_PEER_STATE_CONNECTED || currentPeer2->data == NULL) continue;
															if (isHere(currentPeer, currentPeer2)) {
																gamepacket_t p(0, static_cast<PlayerInfo*>(currentPeer->data)->netID);
																p.Insert("OnGuildDataChanged");
																p.Insert(36095);
																p.Insert(63158650);
																p.Insert((65536 * static_cast<PlayerInfo*>(currentPeer->data)->guildBg) + static_cast<PlayerInfo*>(currentPeer->data)->guildFg);
																p.Insert(static_cast<PlayerInfo*>(currentPeer->data)->guild_rank);
																p.CreatePacket(currentPeer2);
															}
														}
													}
												}
												ifstream read_guild("save/guilds/_" + guild_name + ".json");
												if (!read_guild.is_open()) {
													break;
												}
												json js;
												read_guild >> js;
												read_guild.close();
												js["foreground"] = 0;
												js["background"] = 0;
												ofstream write_guild("save/guilds/_" + guild_name + ".json");
												write_guild << js << std::endl;
												write_guild.close();
											}
											catch (std::exception& e) {
												std::cout << e.what() << std::endl;
												break;
											}
											break;
										}
										if (infoDat.at(0) == "guild_fg") {
											try {
												auto iscontains = false;
												SearchInventoryItem(peer, atoi(infoDat.at(1).c_str()), 1, iscontains);
												if (!iscontains) break;
												ifstream read_player("save/players/_" + world->owner + ".json");
												if (!read_player.is_open()) {
													break;
												}
												json j;
												read_player >> j;
												read_player.close();
												string guild_name = j["guild"];
												for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
													if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
													if (static_cast<PlayerInfo*>(currentPeer->data)->guild == guild_name) {
														static_cast<PlayerInfo*>(currentPeer->data)->guildFg = atoi(infoDat.at(1).c_str());
														Player::OnTalkBubble(currentPeer, static_cast<PlayerInfo*>(currentPeer->data)->netID, "Your guild foreground have been changed to " + getItemDef(atoi(infoDat.at(1).c_str())).name, 0, true);
														Player::OnConsoleMessage(currentPeer, "`oYour guild foreground have been changed to " + getItemDef(atoi(infoDat.at(1).c_str())).name);
														Player::PlayAudio(currentPeer, "audio/siren.wav", 0);
														for (ENetPeer* currentPeer2 = server->peers; currentPeer2 < &server->peers[server->peerCount]; ++currentPeer2) {
															if (currentPeer2->state != ENET_PEER_STATE_CONNECTED || currentPeer2->data == NULL) continue;
															if (isHere(currentPeer, currentPeer2)) {
																gamepacket_t p(0, static_cast<PlayerInfo*>(currentPeer->data)->netID);
																p.Insert("OnGuildDataChanged");
																p.Insert(36095);
																p.Insert(63158650);
																p.Insert((65536 * static_cast<PlayerInfo*>(currentPeer->data)->guildBg) + static_cast<PlayerInfo*>(currentPeer->data)->guildFg);
																p.Insert(static_cast<PlayerInfo*>(currentPeer->data)->guild_rank);
																p.CreatePacket(currentPeer2);
															}
														}
													}
												}
												ifstream read_guild("save/guilds/_" + guild_name + ".json");
												if (!read_guild.is_open()) {
													break;
												}
												json js;
												read_guild >> js;
												read_guild.close();
												js["foreground"] = atoi(infoDat.at(1).c_str());
												ofstream write_guild("save/guilds/_" + guild_name + ".json");
												write_guild << js << std::endl;
												write_guild.close();
											}
											catch (std::exception& e) {
												std::cout << e.what() << std::endl;
												break;
											}
											break;
										}
										if (infoDat.at(0) == "guild_bg") {
											try {
												auto iscontains = false;
												SearchInventoryItem(peer, atoi(infoDat.at(1).c_str()), 1, iscontains);
												if (!iscontains) break;
												ifstream read_player("save/players/_" + world->owner + ".json");
												if (!read_player.is_open()) {
													break;
												}
												json j;
												read_player >> j;
												read_player.close();
												string guild_name = j["guild"];
												for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
													if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
													if (static_cast<PlayerInfo*>(currentPeer->data)->guild == guild_name) {
														static_cast<PlayerInfo*>(currentPeer->data)->guildBg = atoi(infoDat.at(1).c_str());
														Player::OnTalkBubble(currentPeer, static_cast<PlayerInfo*>(currentPeer->data)->netID, "Your guild background have been changed to " + getItemDef(atoi(infoDat.at(1).c_str())).name, 0, true);
														Player::OnConsoleMessage(currentPeer, "`oYour guild background have been changed to " + getItemDef(atoi(infoDat.at(1).c_str())).name);
														Player::PlayAudio(currentPeer, "audio/siren.wav", 0);
														for (ENetPeer* currentPeer2 = server->peers; currentPeer2 < &server->peers[server->peerCount]; ++currentPeer2) {
															if (currentPeer2->state != ENET_PEER_STATE_CONNECTED || currentPeer2->data == NULL) continue;
															if (isHere(currentPeer, currentPeer2)) {
																gamepacket_t p(0, static_cast<PlayerInfo*>(currentPeer->data)->netID);
																p.Insert("OnGuildDataChanged");
																p.Insert(36095);
																p.Insert(63158650);
																p.Insert((65536 * static_cast<PlayerInfo*>(currentPeer->data)->guildBg) + static_cast<PlayerInfo*>(currentPeer->data)->guildFg);
																p.Insert(static_cast<PlayerInfo*>(currentPeer->data)->guild_rank);
																p.CreatePacket(currentPeer2);
															}
														}
													}
												}
												ifstream read_guild("save/guilds/_" + guild_name + ".json");
												if (!read_guild.is_open()) {
													break;
												}
												json js;
												read_guild >> js;
												read_guild.close();
												js["background"] = atoi(infoDat.at(1).c_str());
												ofstream write_guild("save/guilds/_" + guild_name + ".json");
												write_guild << js << std::endl;
												write_guild.close();
											}
											catch (std::exception& e) {
												std::cout << e.what() << std::endl;
												break;
											}
											break;
										}
										if (infoDat.at(1) == "changecat") {
											Player::OnDialogRequest(peer, "set_default_color|`o\nadd_label_with_icon|big|`wSet World Category``|left|3802|\nadd_textbox|Select a category for your world.|left|\nadd_button|None|None|noflags|0|0|\nadd_button|Mining|Mining|noflags|0|0|\nadd_button|Adventure|Adventure|noflags|0|0|\nadd_button|Art|Art|noflags|0|0|\nadd_button|Farm|Farm|noflags|0|0|\nadd_button|Game|Game|noflags|0|0|\nadd_button|Information|Information|noflags|0|0|\nadd_button|Music|Music|noflags|0|0|\nadd_button|Parkour|Parkour|noflags|0|0|\nadd_button|Puzzle|Puzzle|noflags|0|0|\nadd_button|Roleplay|Roleplay|noflags|0|0|\nadd_button|Shop|Shop|noflags|0|0|\nadd_button|Social|Social|noflags|0|0|\nadd_button|Storage|Storage|noflags|0|0|\nadd_button|Story|Story|noflags|0|0|\nadd_button|Trade|Trade|noflags|0|0|\nadd_smalltext|Worlds in the category 'None' can't be rated by players, and they are only listed on the normal World Select screen.|left|\nadd_smalltext|`4Warning:`` Changing your category will delete all ratings on your world.|left|\nend_dialog|worldcategory|Nevermind||");
											break;
										}
										if (infoDat.at(0) == "checkbox_public") {
											int x = pData->wrenchedBlockLocation % world->width;
											int y = pData->wrenchedBlockLocation / world->width;
											if (world->items.at(x + (y * world->width)).foreground == 202 || world->items.at(x + (y * world->width)).foreground == 204 || world->items.at(x + (y * world->width)).foreground == 206 || world->items.at(x + (y * world->width)).foreground == 4994) {
												if (pData->rawName == world->items.at(x + (y * world->width)).monitorname || mod(peer) || isWorldOwner(peer, world)) {
													if (!world->items.at(x + (y * world->width)).opened && infoDat.at(1) == "1") world->items.at(x + (y * world->width)).opened = true;
													else if (infoDat.at(1) == "0" && world->items.at(x + (y * world->width)).opened) world->items.at(x + (y * world->width)).opened = false;
												}
												for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
													if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
													if (isHere(peer, currentPeer)) {
														if (world->items.at(x + (y * world->width)).opened) apply_lock_packet(world, currentPeer, x, y, world->items.at(x + (y * world->width)).foreground, -3);
														else if (world->items.at(x + (y * world->width)).monitorname == static_cast<PlayerInfo*>(currentPeer->data)->rawName) apply_lock_packet(world, currentPeer, x, y, world->items.at(x + (y * world->width)).foreground, static_cast<PlayerInfo*>(currentPeer->data)->netID);
														else apply_lock_packet(world, currentPeer, x, y, world->items.at(x + (y * world->width)).foreground, -1);
													}
												}
											}
											else {
												if (infoDat.at(1) == "1" && !world->isPublic) {
													world->isPublic = true;
													for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
														if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
														if (isHere(peer, currentPeer)) {
															Player::OnConsoleMessage(currentPeer, "World is now public, everyone can place and break");
														}
													}
												}
												else if (infoDat.at(1) == "0" && world->isPublic) {
													world->isPublic = false;
													for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
														if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
														if (isHere(peer, currentPeer)) {
															Player::OnConsoleMessage(currentPeer, "World is no longer public, everyone cannot place and break");
														}
													}
												}
											}
										}
										if (infoDat.at(0) == "playerNetID") {
											playerNetId = infoDat.at(1);
										}
										if (infoDat.at(0) == "checkbox_muted") {
											muted = infoDat.at(1);
										}
										if (infoDat.at(0) == "checkbox_dropgem") {
											if (world->rating == 1 && infoDat.at(1) == "0") {
												world->rating = 0;
												world->drop_gem = true;
											}
											else if (world->rating == 0 && infoDat.at(1) == "1") {
												world->rating = 1;
												world->drop_gem = false;
											}
										}
										if (infoDat.at(0) == "checkbox_rainbow") {
											int x = pData->wrenchedBlockLocation % world->width;
											int y = pData->wrenchedBlockLocation / world->width;
											if (infoDat.at(1) == "1" && !world->rainbow) {
												world->rainbow = true;
											}
											else if (infoDat.at(1) == "0" && world->rainbow) {
												world->rainbow = false;
											}
											for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
												if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
												if (isHere(peer, currentPeer)) {
													if (world->rainbow) send_rainbow_shit_data(currentPeer, world->items.at(x + (y * world->width)).foreground, world->items.at(x + (y * world->width)).background, x, y, true, pData->netID);
													else send_rainbow_shit_data(currentPeer, world->items.at(x + (y * world->width)).foreground, world->items.at(x + (y * world->width)).background, x, y, false, pData->netID);
												}
											}
										}
										if (infoDat.at(0).substr(0, 9) == "checkbox_") {
											string name = infoDat.at(0).substr(infoDat.at(0).find("_") + 1);
											try {
												if (isWorldAdmin(name, world) && infoDat.at(1) == "0") {
													ifstream read_player("save/players/_" + name + ".json");
													if (!read_player.is_open()) {
														break;
													}
													json j;
													read_player >> j;
													read_player.close();
													string username = j["nick"];
													int adminLevel = j["adminLevel"];
													if (username == "") {
														username = role_prefix.at(adminLevel) + name;
													}
													world->accessed.erase(std::remove(world->accessed.begin(), world->accessed.end(), name), world->accessed.end());
													for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
														if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
														if (isHere(peer, currentPeer)) {
															Player::OnConsoleMessage(peer, "" + username + " `owas removed from a World Lock.");
														}
													}
													string current_peer_nick = "";
													for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
														if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
														if (static_cast<PlayerInfo*>(currentPeer->data)->rawName == name) {
															Player::OnConsoleMessage(currentPeer, "You lost access to World Lock in " + pData->currentWorld);
															if (static_cast<PlayerInfo*>(currentPeer->data)->currentWorld == pData->currentWorld) {
																size_t pos;
																while ((pos = static_cast<PlayerInfo*>(currentPeer->data)->displayName.find("`^")) != string::npos) {
																	static_cast<PlayerInfo*>(currentPeer->data)->displayName.replace(pos, 2, "");
																}
																Player::OnNameChanged(peer, static_cast<PlayerInfo*>(currentPeer->data)->netID, static_cast<PlayerInfo*>(currentPeer->data)->displayName);
																int x = pData->wrenchedBlockLocation % world->width;
																int y = pData->wrenchedBlockLocation / world->width;
																PlayerMoving moving{};
																moving.packetType = 0x3;
																moving.characterState = 0x0;
																moving.x = x;
																moving.y = y;
																moving.punchX = x;
																moving.punchY = y;
																moving.XSpeed = 0;
																moving.YSpeed = 0;
																moving.netID = -1;
																moving.plantingTree = world->items.at(pData->wrenchedBlockLocation).foreground;
																SendPacketRaw(4, packPlayerMoving(&moving), 56, nullptr, currentPeer, ENET_PACKET_FLAG_RELIABLE);
															}
														}
													}
												}
											}
											catch (std::exception& e) {
												std::cout << e.what() << std::endl;
												break;
											}
										}
										if (infoDat.at(0) == "minimum_entry_level") {
											int x = pData->wrenchedBlockLocation % world->width;
											int y = pData->wrenchedBlockLocation / world->width;
											if (x < 0 || y < 0) break;
											for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer)
											{
												if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
												if (isHere(peer, currentPeer))
												{
													if (static_cast<PlayerInfo*>(currentPeer->data)->netID == atoi(playerNetId.c_str()))
													{
														accessPlayer(peer, static_cast<PlayerInfo*>(currentPeer->data)->rawName, world);
														break;
													}
												}
											}
											if (muted == "1") world->silence = true;
											else world->silence = false;
											if (world->entrylevel != atoi(infoDat.at(1).c_str())) {
												if (atoi(infoDat.at(1).c_str()) >= 125 || infoDat.at(1).size() > 3) {
													Player::OnTalkBubble(peer, pData->netID, "Minimum entry level can't be higher than 124.", 0, 0);
												}
												else {
													world->entrylevel = atoi(infoDat.at(1).c_str());
													Player::OnTalkBubble(peer, pData->netID, "Minimum entry level for this world is set to `2Level " + infoDat.at(1) + "``.", 0, 0);
												}
											}
											break;
										}
									}
									if (isHeatwaveDialog) {
										if (infoDat.at(0) == "heatwave_red") h_r = infoDat.at(1);
										if (infoDat.at(0) == "heatwave_green") h_g = infoDat.at(1);
										if (infoDat.at(0) == "heatwave_blue") h_b = infoDat.at(1);
									}
									if (isStuffDialog) {
										if (infoDat.at(0) == "choose") stuffITEM = infoDat.at(1);
										if (infoDat.at(0) == "gravity") stuff_gravity = infoDat.at(1);
										if (infoDat.at(0) == "spin") stuff_spin = infoDat.at(1);
										if (infoDat.at(0) == "invert") stuff_invert = infoDat.at(1);
									}
									if (isHeatwaveDialog) {
										if (h_b != "") {
											int x = pData->wrenchedBlockLocation % world->width;
											int y = pData->wrenchedBlockLocation / world->width;
											if (world->isPublic || isWorldAdmin(peer, world) || isWorldOwner(peer, world) || world->owner == "" || admin(peer) || !restricted_area(peer, world, x, y)) {
												int current_r = world->items.at(x + (y * world->width)).vid;
												int current_g = world->items.at(x + (y * world->width)).vprice;
												int current_b = world->items.at(x + (y * world->width)).vcount;
												if (atoi(h_r.c_str()) == current_r && atoi(h_g.c_str()) == current_g && atoi(h_b.c_str()) == current_b) break;
												world->items.at(x + (y * world->width)).vid = atoi(h_r.c_str());
												world->items.at(x + (y * world->width)).vprice = atoi(h_g.c_str());
												world->items.at(x + (y * world->width)).vcount = atoi(h_b.c_str());
												sendHeatwave(peer, x, y, world->items.at(x + (y * world->width)).vid, world->items.at(x + (y * world->width)).vprice, world->items.at(x + (y * world->width)).vcount);
												if (world->items.at(x + (y * world->width)).activated) {
													for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
														if (currentPeer->state != ENET_PEER_STATE_CONNECTED) continue;
														if (isHere(peer, currentPeer)) {
															Player::OnSetCurrentWeather(currentPeer, 28);
														}
													}
												}
											}
											break;
										}
									}
									if (isBackgroundDialog) {
										if (infoDat.at(0) == "choose") {
											int x = pData->wrenchedBlockLocation % world->width;
											int y = pData->wrenchedBlockLocation / world->width;
											if (world->isPublic || isWorldAdmin(peer, world) || isWorldOwner(peer, world) || world->owner == "" || admin(peer) || !restricted_area(peer, world, x, y)) {
												int itemid = atoi(infoDat.at(1).c_str());
												if (world->items.at(x + (y * world->width)).intdata == itemid || infoDat.at(1) == "") {
													break;
												}
												if (getItemDef(itemid).blockType != BlockTypes::BACKGROUND) {
													Player::OnTalkBubble(peer, pData->netID, "That's not a background!", 0, true);
													break;
												}
												world->items.at(x + (y * world->width)).intdata = itemid;
												sendBackground(peer, x, y, itemid);
												if (world->items.at(x + (y * world->width)).activated) {
													for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
														if (currentPeer->state != ENET_PEER_STATE_CONNECTED) continue;
														if (isHere(peer, currentPeer)) {
															Player::OnSetCurrentWeather(currentPeer, 34);
														}
													}
												}
											}
											break;
										}
									}
									if (isStuffDialog) {
										if (stuff_gravity != "" && stuff_spin != "" && stuff_invert != "") {
											int x = pData->wrenchedBlockLocation % world->width;
											int y = pData->wrenchedBlockLocation / world->width;
											if (world->isPublic || isWorldAdmin(peer, world) || isWorldOwner(peer, world) || world->owner == "" || admin(peer) || !restricted_area(peer, world, x, y)) {
												bool invert = false;
												if (stuff_invert == "1") invert = true;
												bool spin = false;
												if (stuff_spin == "1") spin = true;
												if (world->items.at(x + (y * world->width)).mc == atoi(stuff_gravity.c_str()) && world->items.at(x + (y * world->width)).opened == invert && world->items.at(x + (y * world->width)).rm == spin) {
													if (world->items.at(x + (y * world->width)).intdata == atoi(stuffITEM.c_str()) || stuffITEM == "") {
														break;
													}
												}
												if (stuffITEM != "") {
													world->items.at(x + (y * world->width)).intdata = atoi(stuffITEM.c_str());
												}
												world->items.at(x + (y * world->width)).mc = atoi(stuff_gravity.c_str());
												world->items.at(x + (y * world->width)).opened = invert;
												world->items.at(x + (y * world->width)).rm = spin;
												sendStuffweather(peer, x, y, world->items.at(x + (y * world->width)).intdata, world->items.at(x + (y * world->width)).mc, world->items.at(x + (y * world->width)).rm, world->items.at(x + (y * world->width)).opened);
												if (world->items.at(x + (y * world->width)).activated) {
													for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
														if (currentPeer->state != ENET_PEER_STATE_CONNECTED) continue;
														if (isHere(peer, currentPeer)) {
															Player::OnSetCurrentWeather(currentPeer, 29);
														}
													}
												}
											}
											break;
										}
									}
									if (isMagCheckDialog) {
										if (infoDat.at(0) == "checkbox_gemcollection") {
											if (infoDat.at(1) == "1") {
												int x = pData->lastPunchX;
												int y = pData->lastPunchY;
												world->items.at(x + (y * world->width)).mid = 112;
												ENetPeer* currentPeer;
												for (currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
													if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
													if (isHere(peer, currentPeer)) {
														send_item_sucker(currentPeer, 5638, x, y, 112, 0, true, true, world->items.at(x + (y * world->width)).background);
													}
												}
											}
											break;
										}
									}
									if (isMagDialog) {
										if (world == nullptr || pData->currentWorld == "EXIT" || serverIsFrozen || pData->rawName != world->owner && !mod(peer) && world->owner != "") break;
										int magplantitemi = 0;
										int x = pData->lastPunchX;
										int y = pData->lastPunchY;
										int suckerid = pData->suckerid;
										string suckername = pData->suckername;
										if (infoDat.at(0) == "magplantitem") magplantitem = infoDat.at(1);
										magplantitemi = atoi(magplantitem.c_str());
										bool allow = true;
										for (int i = 0; i < world->width * world->height; i++) {
											if (world->items.at(i).foreground == 5638 || world->items.at(i).foreground == 6946 || world->items.at(i).foreground == 6948) {
												string SuckerName3 = "";
												if (world->items.at(i).foreground == 5638) SuckerName3 = "magplant";
												if (world->items.at(i).foreground == 6946) SuckerName3 = "gaiabeacon";
												if (world->items.at(i).foreground == 6948) SuckerName3 = "unstabletesseract";
												if (magplantitemi == world->items.at(i).mid) allow = false;
											}
										}
										if (magplantitemi == 18 || magplantitemi == 32 || getItemDef(magplantitemi).properties & Property_Untradable) {
											Player::OnTalkBubble(peer, pData->netID, "You cannot use this item in " + getItemDef(suckerid).name + "!", 0, true);
										}
										else if (suckerid == 6946 && !isSeed(magplantitemi)) {
											Player::OnTalkBubble(peer, pData->netID, getItemDef(suckerid).name + " supports only seeds!", 0, true);
										}
										else if (suckerid == 6948 && getItemDef(magplantitemi).blockType != BlockTypes::FOREGROUND && getItemDef(magplantitemi).blockType != BlockTypes::PAIN_BLOCK && getItemDef(magplantitemi).blockType != BlockTypes::GROUND_BLOCK && getItemDef(magplantitemi).blockType != BlockTypes::BACKGROUND) {
											Player::OnTalkBubble(peer, pData->netID, getItemDef(suckerid).name + " supports only foregrounds or backgrounds!", 0, true);
										}
										else if (allow == false) {
											Player::OnTalkBubble(peer, pData->netID, "There is already Item Sucker with such item!", 0, true);
										}
										else {
											auto contains = false;
											SearchInventoryItem(peer, magplantitemi, 1, contains);
											if (!contains) break;
											ENetPeer* currentPeer;
											for (currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
												if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
												if (isHere(peer, currentPeer)) {
													send_item_sucker(currentPeer, suckerid, x, y, magplantitemi, 0, true, true, world->items.at(x + (y * world->width)).background);
												}
											}
											world->items.at(x + (y * world->width)).mid = magplantitemi;
											world->items.at(x + (y * world->width)).mc = 0;
										}
									}
									if (isTeleDialog)
									{
										if (world == nullptr || pData->currentWorld == "EXIT" || serverIsFrozen) continue;
										if (infoDat.at(0) == "telephonenumber")
										{
											Player::OnTextOverlay(peer, "Salesman under development!");
											break;
											telefonas = infoDat.at(1);
											if (telefonas.size() > 5 || telefonas.size() <= 0)
											{
												continue;
											}
											if (telefonas != "12345" && telefonas != "53785")
											{
												Player::OnTextOverlay(peer, "This phone number does not exist!");
											}
											else if (telefonas == "12345")
											{
												Player::OnDialogRequest(peer, "set_default_color|`o\n\nadd_label_with_icon|big|`wLocke The Salesman``|left|2398|0|0|\nadd_spacer|small|\nadd_label|small|\nadd_textbox|`oI'm selling all kind of `9Stuff `oFor `9Various Items `oTell Me, Which kind of `9Stuff `oDo you want to `^Buy`o?|\nadd_spacer|small|\nadd_button|purchaseyellowcrystal|`oPurchase `9Yellow Crystal `o(`@15 `2Green Crystals`o)|\nadd_button|purchaselegendaryorb|`oPurchase `8Legendary Orb `o(`@1 `cSummer Clash Ticket`o)|\nadd_spacer|small|\nadd_button|store|`#Back|0|0|\n\nadd_quick_exit|\nnend_dialog|gazette||OK|");
											}
											else
											{
												Player::OnDialogRequest(peer, "set_default_color|`o\nadd_label_with_icon|big|`wSales-Man``|left|4358|\nadd_textbox|It is I, Sales-Man, savior of the wealthy! Let me rescue you from your riches. What would you like to buy today?|left|\nadd_button|chc0|Surgery Items|noflags|0|0|\nadd_button|chc1|Wolfworld Items|noflags|0|0|\nadd_button|chc2|Zombie Defense Items|noflags|0|0|\nadd_button|chc3|Cooking Items|noflags|0|0|\nadd_button|chc4|Startopia Items|noflags|0|0|\nadd_button|chc5|Blue Gem Lock|noflags|0|0|\nend_dialog|phonecall|Hang Up||");
											}
										}
									}
									if (isCaveDialog) {
										if (infoDat.at(0) == "cavename") {
											string upsd = infoDat.at(1);
											string act = infoDat.at(1);
											std::transform(upsd.begin(), upsd.end(), upsd.begin(), ::toupper);
											if (act == "" || act == "\\" || act.find("\\") != string::npos || act.find(" ") != string::npos || act.find("  ") != string::npos || act.find(".") != string::npos || act.find("?") != string::npos || act.find(",") != string::npos || act.find("@") != string::npos || act.find("[") != string::npos || act.find("]") != string::npos || act.find("#") != string::npos || act.find("<") != string::npos || act.find(">") != string::npos || act.find(":") != string::npos || act.find("\"") != string::npos || act.find("{") != string::npos || act.find("}") != string::npos || act.find("|") != string::npos || act.find("+") != string::npos || act.find("_") != string::npos || act.find("~") != string::npos || act.find("-") != string::npos || act.find("!") != string::npos || act.find("$") != string::npos || act.find("%") != string::npos || act.find("^") != string::npos || act.find("&") != string::npos || act.find("`") != string::npos || act.find("*") != string::npos || act.find("(") != string::npos || act.find(")") != string::npos || act.find("=") != string::npos || act.find("'") != string::npos || act.find(";") != string::npos || act.find("/") != string::npos) {
												Player::OnTalkBubble(peer, pData->netID, "Sorry, a world name can't contain special characters or spaces.", 0, false);
												continue;
											}
											if (!world_exist(upsd)) {
												if (upsd.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789") != string::npos) {
													Player::OnTalkBubble(peer, pData->netID, "Sorry, a world name can't contain special characters or spaces.", 0, false);
													continue;
												}
												if (upsd.find(" ") != string::npos) {
													Player::OnTalkBubble(peer, pData->netID, "Sorry, a world name can't contain special characters or spaces.", 0, false);
													continue;
												}
												if (upsd == "EXIT") {
													Player::OnTalkBubble(peer, pData->netID, "That world name already exists.  You'll have to be more original.  Maybe add some numbers after it?", 0, false);
													Player::OnConsoleMessage(peer, "That world name already exists.  You'll have to be more original.  Maybe add some numbers after it?");
													continue;
												}
												if (upsd == pData->currentWorld) {
													Player::OnTalkBubble(peer, pData->netID, "That world name already exists.  You'll have to be more original.  Maybe add some numbers after it?", 0, false);
													Player::OnConsoleMessage(peer, "That world name already exists.  You'll have to be more original.  Maybe add some numbers after it?");
													continue;
												}
												RemoveInventoryItem(3562, 1, peer, true);
												usedcaveblast = true;
												WorldInfo info = worldDB.get(upsd);
												if (info.name == "error") {
													Player::OnConsoleMessage(peer, "An error occured while trying to enter this world, please try again!");
													continue;
												}
												string act = upsd;
												for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
													if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
													if (isHere(peer, currentPeer)) {
														Player::OnTalkBubble(currentPeer, pData->netID, "`5** " + pData->displayName + " `5activates a Cave Blast! `5**", 0, false);
														Player::OnConsoleMessage(currentPeer, "`5** " + pData->displayName + " `5activates a Cave Blast! `5**");
													}
												}
												handle_world(peer, act);
											}
											else {
												Player::OnTalkBubble(peer, pData->netID, "That world name already exists.  You'll have to be more original.  Maybe add some numbers after it?", 0, false);
												Player::OnConsoleMessage(peer, "That world name already exists.  You'll have to be more original.  Maybe add some numbers after it?");
											}
										}
									}
									if (isThermoDialog) {
										if (infoDat.at(0) == "thermoname") {
											thermonames = infoDat.at(1);
											string upsd = thermonames;
											string act = thermonames;
											std::transform(upsd.begin(), upsd.end(), upsd.begin(), ::toupper);
											if (act == "" || act == "\\" || act.find("\\") != string::npos || act.find(" ") != string::npos || act.find("  ") != string::npos || act.find(".") != string::npos || act.find("?") != string::npos || act.find(",") != string::npos || act.find("@") != string::npos || act.find("[") != string::npos || act.find("]") != string::npos || act.find("#") != string::npos || act.find("<") != string::npos || act.find(">") != string::npos || act.find(":") != string::npos || act.find("\"") != string::npos || act.find("{") != string::npos || act.find("}") != string::npos || act.find("|") != string::npos || act.find("+") != string::npos || act.find("_") != string::npos || act.find("~") != string::npos || act.find("-") != string::npos || act.find("!") != string::npos || act.find("$") != string::npos || act.find("%") != string::npos || act.find("^") != string::npos || act.find("&") != string::npos || act.find("`") != string::npos || act.find("*") != string::npos || act.find("(") != string::npos || act.find(")") != string::npos || act.find("=") != string::npos || act.find("'") != string::npos || act.find(";") != string::npos || act.find("/") != string::npos) {
												Player::OnTalkBubble(peer, pData->netID, "Sorry, a world name can't contain special characters or spaces.", 0, false);
												continue;
											}
											if (!world_exist(upsd)) {
												if (upsd.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789") != string::npos) {
													Player::OnTalkBubble(peer, pData->netID, "Sorry, a world name can't contain special characters or spaces.", 0, false);
													continue;
												}
												if (upsd.find(" ") != string::npos) {
													Player::OnTalkBubble(peer, pData->netID, "Sorry, a world name can't contain special characters or spaces.", 0, false);
													continue;
												}
												if (upsd == "EXIT") {
													Player::OnTalkBubble(peer, pData->netID, "That world name already exists.  You'll have to be more original.  Maybe add some numbers after it?", 0, false);
													Player::OnConsoleMessage(peer, "That world name already exists.  You'll have to be more original.  Maybe add some numbers after it?");
													continue;
												}
												if (upsd == pData->currentWorld) {
													Player::OnTalkBubble(peer, pData->netID, "That world name already exists.  You'll have to be more original.  Maybe add some numbers after it?", 0, false);
													Player::OnConsoleMessage(peer, "That world name already exists.  You'll have to be more original.  Maybe add some numbers after it?");
													continue;
												}
												RemoveInventoryItem(1402, 1, peer, true);
												usedthermoblast = true;
												WorldInfo info = worldDB.get(thermonames);
												if (info.name == "error") {
													Player::OnConsoleMessage(peer, "An error occured while trying to enter this world, please try again!");
													continue;
												}
												string act = thermonames;
												for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
													if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
													if (isHere(peer, currentPeer)) {
														Player::OnTalkBubble(currentPeer, pData->netID, "`5** " + pData->displayName + " `5activates a Thermonuclear Blast! `5**", 0, false);
														Player::OnConsoleMessage(currentPeer, "`5** " + pData->displayName + " `5activates a Thermonuclear Blast! `5**");
													}
												}
												handle_world(peer, act);
											}
											else {
												Player::OnTalkBubble(peer, pData->netID, "That world name already exists.  You'll have to be more original.  Maybe add some numbers after it?", 0, false);
												Player::OnConsoleMessage(peer, "That world name already exists.  You'll have to be more original.  Maybe add some numbers after it?");
											}
										}
									}
									if (isGuildDialog) {
										if (world == nullptr || pData->currentWorld == "EXIT" || serverIsFrozen) continue;
										if (infoDat.at(0) == "gname") guild_name = infoDat.at(1);
										if (infoDat.at(0) == "gstatement") guild_statement = infoDat.at(1);
										if (infoDat.at(0) == "ggcflagbg") guild_mascot_bg = infoDat.at(1);
										if (infoDat.at(0) == "ggcflagfg") {
											guild_mascot_fg = infoDat.at(1);
											int GCState = PlayerDB::guildRegister(peer, guild_name, guild_statement, guild_mascot_bg, guild_mascot_fg);
											switch (GCState) {
											case -1:
											{
												Player::OnConsoleMessage(peer, "");
												break;
											}
											case -2:
											{
												Player::OnConsoleMessage(peer, "");
												break;
											}
											case -3:
											{
												Player::OnConsoleMessage(peer, "");
												break;
											}
											case -4:
											{
												Player::OnConsoleMessage(peer, "");
												break;
											}
											case -5:
											{
												Player::OnConsoleMessage(peer, "");
												break;
											}
											case -6:
											{
												Player::OnConsoleMessage(peer, "");
												break;
											}
											case -7:
											{
												Player::OnConsoleMessage(peer, "");
												break;
											}
											case -8:
											{
												Player::OnConsoleMessage(peer, "");
												break;
											}
											case 1:
											{
												Player::OnConsoleMessage(peer, "");
												break;
											}
											default: break;
											}
										}
									}
									if (isRegisterDialog) {
										if (infoDat.at(0) == "username") register_username = infoDat.at(1);
										if (infoDat.at(0) == "password") register_password = infoDat.at(1);
										if (infoDat.at(0) == "password_verify") register_password_verify = infoDat.at(1);
										if (infoDat.at(0) == "email") {
											register_email = infoDat.at(1);
											if (register_password != register_password_verify) {
												auto p2 = packetEnd(appendString(appendString(createPacket(), "OnDialogRequest"), "text_scaling_string||\nset_default_color|`o\nadd_label_with_icon|big|`wGet a GrowID``|left|206|\nadd_spacer|small|\nadd_textbox|`4These Passwords don't match up. Try again..!|\nadd_spacer|small|\nadd_textbox|By choosing a `wGrowID``, you can use a name and password to logon from any device. Your `wname`` will be shown to other players!|left|\nadd_spacer|small|\nadd_text_input|username|Name||18|\nadd_textbox|Your `wpassword`` must contain `w8 to 18 characters, 1 letter, 1 number`` and `w1 special character: @#!$^&*.,``|left|\nadd_text_input_password|password|Password||18|\nadd_text_input_password|password_verify|Password Verify||18|\nadd_textbox|Your `wemail`` will only be used for account verification and support. If you enter a fake email, you can't verify your account, recover or change your password.|left|\nadd_text_input|email|Email||64|\nadd_textbox|We will never ask you for your password or email, never share it with anyone!|left|\nend_dialog|register|Cancel|Get My GrowID!|\n"));
												auto respawnTimeout = 500;
												auto deathFlag = 0x19;
												memcpy(p2.data + 24, &respawnTimeout, 4);
												memcpy(p2.data + 56, &deathFlag, 4);
												const auto packet2 = enet_packet_create(p2.data, p2.len, ENET_PACKET_FLAG_RELIABLE);
												enet_peer_send(peer, 0, packet2);
												delete p2.data;
												break;
											}
											auto regState = PlayerDB::playerRegister(peer, register_username, register_password, register_email);
											switch (regState) {
											case 1:
											{
												GamePacket p8 = packetEnd(appendString(appendString(appendInt(appendString(createPacket(), "SetHasGrowID"), 1), register_username), register_password));
												ENetPacket* packet8 = enet_packet_create(p8.data, p8.len, ENET_PACKET_FLAG_RELIABLE);
												enet_peer_send(peer, 0, packet8);
												delete p8.data;
												pData->displayName = register_username;
												pData->tankIDName = register_username;
												pData->tankIDPass = register_password;
												pData->rawName = PlayerDB::getProperName(pData->tankIDName);
												pData->msgName = PlayerDB::getProperName(pData->tankIDName);
												pData->haveGrowId = true;
												pData->HasLogged = true;
												pData->effect = 8421376;
												Player::OnDialogRequest(peer, "set_default_color|`o\nadd_label_with_icon|big|`wGrowID GET!``|left|206|\nadd_textbox|A `wGrowID`` with the log on of `w" + pData->displayName + "`` created.  Write it and your password down as they will be required to log on from now on!|left|\nend_dialog|growid_apply|Continue||");
												Player::PlayAudio(peer, "audio/piano_nice.wav", 150);
												ifstream read_entry("config+/register/starting_gems.json");
												json j;
												read_entry >> j;
												read_entry.close();
												int StartingGems = j;
												ofstream myfile;
												myfile.open("save/gemdb/_" + pData->rawName + ".txt");
												myfile << StartingGems;
												myfile.close();
												Player::OnSetBux(peer, StartingGems, 1);
												Player::OnNameChanged(peer, pData->netID, register_username);
												LoadPlayerData(peer);
												DailyRewardCheck(peer);
												break;
											}
											case -1:
											{
												auto p2 = packetEnd(appendString(appendString(createPacket(), "OnDialogRequest"), "text_scaling_string||\nset_default_color|`o\nadd_label_with_icon|big|`wGet a GrowID``|left|206|add_spacer|small|add_textbox|`4Sorry but someone else has `w" + register_username + " `4 as there username!. Try again..!|add_spacer|small|\nadd_textbox|By choosing a `wGrowID``, you can use a name and password to logon from any device. Your `wname`` will be shown to other players!|left|\nadd_spacer|small|\nadd_text_input|username|Name||18|\nadd_textbox|Your `wpassword`` must contain `w8 to 18 characters, 1 letter, 1 number`` and `w1 special character: @#!$^&*.,``|left|\nadd_text_input_password|password|Password||18|\nadd_text_input_password|password_verify|Password Verify||18|\nadd_textbox|Your `wemail`` will only be used for account verification and support. If you enter a fake email, you can't verify your account, recover or change your password.|left|\nadd_text_input|email|Email||64|\nadd_textbox|We will never ask you for your password or email, never share it with anyone!|left|\nend_dialog|register|Cancel|Get My GrowID!|\n"));
												auto respawnTimeout = 500;
												auto deathFlag = 0x19;
												memcpy(p2.data + 24, &respawnTimeout, 4);
												memcpy(p2.data + 56, &deathFlag, 4);
												const auto packet2 = enet_packet_create(p2.data, p2.len, ENET_PACKET_FLAG_RELIABLE);
												enet_peer_send(peer, 0, packet2);
												delete p2.data;
												break;
											}
											case -2:
											{
												auto p2 = packetEnd(appendString(appendString(createPacket(), "OnDialogRequest"), "text_scaling_string||\nset_default_color|`o\nadd_label_with_icon|big|`wGet a GrowID``|left|206|\nadd_spacer|small|\nadd_textbox|`4Seems like this email does not exist. Try again..!|\nadd_spacer|small|\nadd_textbox|By choosing a `wGrowID``, you can use a name and password to logon from any device. Your `wname`` will be shown to other players!|left|\nadd_spacer|small|\nadd_text_input|username|Name||18|\nadd_textbox|Your `wpassword`` must contain `w8 to 18 characters, 1 letter, 1 number`` and `w1 special character: @#!$^&*.,``|left|\nadd_text_input_password|password|Password||18|\nadd_text_input_password|password_verify|Password Verify||18|\nadd_textbox|Your `wemail`` will only be used for account verification and support. If you enter a fake email, you can't verify your account, recover or change your password.|left|\nadd_text_input|email|Email||64|\nadd_textbox|We will never ask you for your password or email, never share it with anyone!|left|\nend_dialog|register|Cancel|Get My GrowID!|\n"));
												auto respawnTimeout = 500;
												auto deathFlag = 0x19;
												memcpy(p2.data + 24, &respawnTimeout, 4);
												memcpy(p2.data + 56, &deathFlag, 4);
												const auto packet2 = enet_packet_create(p2.data, p2.len, ENET_PACKET_FLAG_RELIABLE);
												enet_peer_send(peer, 0, packet2);
												delete p2.data;
												break;
											}
											case -3:
											{
												auto p2 = packetEnd(appendString(appendString(createPacket(), "OnDialogRequest"), "text_scaling_string||\nset_default_color|`o\nadd_label_with_icon|big|`wGet a GrowID``|left|206|\nadd_spacer|small|\nadd_textbox|`4Sorry but username or password seems to be too long. Try again..!|\nadd_spacer|small|\nadd_textbox|By choosing a `wGrowID``, you can use a name and password to logon from any device. Your `wname`` will be shown to other players!|left|\nadd_spacer|small|\nadd_text_input|username|Name||18|\nadd_textbox|Your `wpassword`` must contain `w8 to 18 characters, 1 letter, 1 number`` and `w1 special character: @#!$^&*.,``|left|\nadd_text_input_password|password|Password||18|\nadd_text_input_password|password_verify|Password Verify||18|\nadd_textbox|Your `wemail`` will only be used for account verification and support. If you enter a fake email, you can't verify your account, recover or change your password.|left|\nadd_text_input|email|Email||64|\nadd_textbox|We will never ask you for your password or email, never share it with anyone!|left|\nend_dialog|register|Cancel|Get My GrowID!|\n"));
												auto respawnTimeout = 500;
												auto deathFlag = 0x19;
												memcpy(p2.data + 24, &respawnTimeout, 4);
												memcpy(p2.data + 56, &deathFlag, 4);
												const auto packet2 = enet_packet_create(p2.data, p2.len, ENET_PACKET_FLAG_RELIABLE);
												enet_peer_send(peer, 0, packet2);
												delete p2.data;
												break;
											}
											case -4:
											{
												auto p2 = packetEnd(appendString(appendString(createPacket(), "OnDialogRequest"), "text_scaling_string||\nset_default_color|`o\nadd_label_with_icon|big|`wGet a GrowID``|left|206|\nadd_spacer|small|\nadd_textbox|`4Username can't contain spaces or symbol. Try again..!|\nadd_spacer|small|\nadd_textbox|By choosing a `wGrowID``, you can use a name and password to logon from any device. Your `wname`` will be shown to other players!|left|\nadd_spacer|small|\nadd_text_input|username|Name||18|\nadd_textbox|Your `wpassword`` must contain `w8 to 18 characters, 1 letter, 1 number`` and `w1 special character: @#!$^&*.,``|left|\nadd_text_input_password|password|Password||18|\nadd_text_input_password|password_verify|Password Verify||18|\nadd_textbox|Your `wemail`` will only be used for account verification and support. If you enter a fake email, you can't verify your account, recover or change your password.|left|\nadd_text_input|email|Email||64|\nadd_textbox|We will never ask you for your password or email, never share it with anyone!|left|\nend_dialog|register|Cancel|Get My GrowID!|\n"));
												auto respawnTimeout = 500;
												auto deathFlag = 0x19;
												memcpy(p2.data + 24, &respawnTimeout, 4);
												memcpy(p2.data + 56, &deathFlag, 4);
												const auto packet2 = enet_packet_create(p2.data, p2.len, ENET_PACKET_FLAG_RELIABLE);
												enet_peer_send(peer, 0, packet2);
												delete p2.data;
												break;
											}
											case -5:
											{
												auto p2 = packetEnd(appendString(appendString(createPacket(), "OnDialogRequest"), "text_scaling_string||\nset_default_color|`o\nadd_label_with_icon|big|`wGet a GrowID``|left|206|\nadd_spacer|small|\nadd_textbox|`4Error06 `wFailed to create account, no valid reasoning just failed! (Contact Leom / Developers for help!). `4Try again..!|\nadd_spacer|small|\nadd_textbox|By choosing a `wGrowID``, you can use a name and password to logon from any device. Your `wname`` will be shown to other players!|left|\nadd_spacer|small|\nadd_text_input|username|Name||18|\nadd_textbox|Your `wpassword`` must contain `w8 to 18 characters, 1 letter, 1 number`` and `w1 special character: @#!$^&*.,``|left|\nadd_text_input_password|password|Password||18|\nadd_text_input_password|password_verify|Password Verify||18|\nadd_textbox|Your `wemail`` will only be used for account verification and support. If you enter a fake email, you can't verify your account, recover or change your password.|left|\nadd_text_input|email|Email||64|\nadd_textbox|We will never ask you for your password or email, never share it with anyone!|left|\nend_dialog|register|Cancel|Get My GrowID!|\n"));
												auto respawnTimeout = 500;
												auto deathFlag = 0x19;
												memcpy(p2.data + 24, &respawnTimeout, 4);
												memcpy(p2.data + 56, &deathFlag, 4);
												const auto packet2 = enet_packet_create(p2.data, p2.len, ENET_PACKET_FLAG_RELIABLE);
												enet_peer_send(peer, 0, packet2);
												delete p2.data;
												break;
											}
											case -6:
											{
												FailLogin(peer, "action|log\nmsg|`4Can not make new account! `oSorry, but IP " + static_cast<PlayerInfo*>(peer->data)->charIP + " is not permitted to create NEW `0GTCP `oaccounts at this time. (this can be because there is an open proxy/VPN here or abuse has come from this IP) Please try again from another IP address.", false);
												break;
											}
											}
										}
									}
								}
							}
						}
						catch (const std::out_of_range& e) {
							std::cout << e.what() << std::endl;
						}
						int x = static_cast<PlayerMoving*>(peer->data)->punchX;
						int y = static_cast<PlayerMoving*>(peer->data)->punchY;
						int causedBy = static_cast<PlayerMoving*>(peer->data)->netID;
						int tile = static_cast<PlayerMoving*>(peer->data)->plantingTree;
						if (btn.substr(0, 23) == "removeselectedbulletin_")
						{
							if (world == nullptr || pData->currentWorld == "EXIT" || serverIsFrozen) continue;
							if (pData->rawName == world->owner || world->owner == "" || mod(peer))
							{
								string coord, apos;
								string coord_and_apos = btn.erase(0, 23);
								string::size_type pos = coord_and_apos.find('_');
								if (pos != std::string::npos)
								{
									coord = coord_and_apos.substr(0, pos);
									coord_and_apos.erase(0, pos + 1);
									apos = coord_and_apos;
								}
								else
								{
									break;
								}
								auto isdbox = std::experimental::filesystem::exists("save/bulletinboard/_" + pData->currentWorld + "/X" + coord + ".json");
								if (!isdbox)
								{
									Player::OnTalkBubble(peer, pData->netID, "`4An error occured while getting safe bulletin board.", 0, true);
									break;
								}

								bool contains_non_int3 = !std::regex_match(apos, std::regex("^[0-9]+$"));
								if (contains_non_int3 == true)
								{
									break;
								}
								int apos_int = atoi(apos.c_str());
								if (apos_int < 1 || apos_int > 90)
								{
									break;
								}
								try
								{
									ifstream ifff("save/bulletinboard/_" + pData->currentWorld + "/X" + coord + ".json");
									json j;
									ifff >> j;
									ifff.close();
									string tekstas = j["mails"][static_cast<nlohmann::basic_json<std::map, std::vector, std::string, bool, int64_t, uint64_t, double, std::allocator, nlohmann::adl_serializer>::size_type>(apos_int) - 1]["text"];
									GTDialog myLetterBox;
									myLetterBox.addLabelWithIcon("`oRemove \"" + tekstas + "\" from your board?", 658, LABEL_BIG);
									myLetterBox.addButton("Confirmremoveselectedbulletin_" + coord + "_" + apos, "`wOK");
									myLetterBox.addSpacer(SPACER_SMALL);
									myLetterBox.addQuickExit();
									myLetterBox.endDialog("Close", "", "Cancel");
									Player::OnDialogRequest(peer, myLetterBox.finishDialog());
								}
								catch (std::exception& e)
								{
									std::cout << e.what() << std::endl;
									break; /*tipo jeigu nera*/
								}
							}
						}
						if (btn.substr(0, 30) == "Confirmremoveselectedbulletin_")
						{
							if (world == nullptr || pData->currentWorld == "EXIT" || serverIsFrozen) continue;
							if (pData->rawName == world->owner || world->owner == "" || mod(peer))
							{
								string coord, apos;
								string coord_and_apos = btn.erase(0, 30);
								string::size_type pos = coord_and_apos.find('_');
								if (pos != std::string::npos)
								{
									coord = coord_and_apos.substr(0, pos);
									coord_and_apos.erase(0, pos + 1);
									apos = coord_and_apos;
								}
								else
								{
									break;
								}
								auto isdbox = std::experimental::filesystem::exists("save/bulletinboard/_" + pData->currentWorld + "/X" + coord + ".json");
								if (!isdbox)
								{
									Player::OnTalkBubble(peer, pData->netID, "`4An error occured while getting safe bulletin board.", 0, true);
									break;
								}

								bool contains_non_int3 = !std::regex_match(apos, std::regex("^[0-9]+$"));
								if (contains_non_int3 == true)
								{
									break;
								}
								int apos_int = atoi(apos.c_str());
								if (apos_int < 1 || apos_int > 90)
								{
									break;
								}
								try
								{
									ifstream ifff("save/bulletinboard/_" + pData->currentWorld + "/X" + coord + ".json");
									json j;
									ifff >> j;
									ifff.close();
									int inmail = j["inmail"];
									j["inmail"] = inmail - 1;
									j["mails"][static_cast<nlohmann::basic_json<std::map, std::vector, std::string, bool, int64_t, uint64_t, double, std::allocator, nlohmann::adl_serializer>::size_type>(apos_int) - 1]["text"] = "";
									j["mails"][static_cast<nlohmann::basic_json<std::map, std::vector, std::string, bool, int64_t, uint64_t, double, std::allocator, nlohmann::adl_serializer>::size_type>(apos_int) - 1]["growid"] = "";
									Player::OnTalkBubble(peer, pData->netID, "`2Bulletin removed.", 0, true);
									ofstream of("save/bulletinboard/_" + pData->currentWorld + "/X" + coord + ".json");
									of << j << std::endl;
									of.close();
								}
								catch (std::exception& e)
								{
									std::cout << e.what() << std::endl;
									break; /*tipo jeigu nera*/
								}
							}
						}
						if (btn.substr(0, 18) == "addbulletinletter_")
						{
							if (world == nullptr || pData->currentWorld == "EXIT" || serverIsFrozen) continue;
							string coord = btn.substr(btn.find("_") + 1);
							auto isdbox = std::experimental::filesystem::exists("save/bulletinboard/_" + pData->currentWorld + "/X" + coord + ".json");
							if (!isdbox)
							{
								Player::OnTalkBubble(peer, pData->netID, "`4An error occured while getting bulletin board info.", 0, true);
								continue;
							}
							try {
								std::stringstream bss(cch);
								std::string bto;

								string addtext = "";

								while (std::getline(bss, bto, '\n'))
								{
									vector<string> infoDat = explode("|", bto);
									if (infoDat.at(0) == "addbulletinletterinput")
									{
										addtext = infoDat.at(1);
									}
								}
								if (addtext.length() < 3)
								{
									Player::OnTalkBubble(peer, pData->netID, "`wThat's not interesting enough to post.", 0, true);
									continue;
								}

								ifstream ifff("save/bulletinboard/_" + pData->currentWorld + "/X" + coord + ".json");
								json j;
								ifff >> j;
								ifff.close();
								if (j["inmail"] >= 90)
								{
									Player::OnTalkBubble(peer, pData->netID, "`4Bulletin Board is full!", 0, true);
									continue;
								}
								for (int i = 0; i < 90; i++)
								{
									if (j["mails"].at(i)["growid"] == "")
									{
										j["mails"].at(i)["growid"] = pData->displayName;
										j["mails"].at(i)["text"] = addtext;
										int inmail = j["inmail"];
										j["inmail"] = inmail + 1;
										Player::OnTalkBubble(peer, pData->netID, "`2Bulletin posted.", 0, true);

										ofstream of("save/bulletinboard/_" + pData->currentWorld + "/X" + coord + ".json");
										of << j << std::endl;
										of.close();

										break;
									}
								}
							}
							catch (std::exception& e)
							{
								std::cout << e.what() << std::endl;
								break; /*tipo jeigu nera*/
							}
						}
						if (btn.substr(0, 17) == "bulletinletterok_")
						{
							if (world == nullptr || pData->currentWorld == "EXIT" || serverIsFrozen) continue;
							string coord = btn.substr(btn.find("_") + 1);
							auto isdbox = std::experimental::filesystem::exists("save/bulletinboard/_" + pData->currentWorld + "/X" + coord + ".json");
							if (!isdbox)
							{
								Player::OnTalkBubble(peer, pData->netID, "`4An error occured while getting bulletin board info.", 0, true);
								continue;
							}
							if (pData->rawName == world->owner || world->owner == "" || mod(peer))
							{
								try {
									std::stringstream bss(cch);
									std::string bto;

									string publiccanadd = "";
									string hidenames = "";

									while (std::getline(bss, bto, '\n'))
									{
										vector<string> infoDat = explode("|", bto);
										if (infoDat.at(0) == "publiccanaddbulletinboard")
										{
											publiccanadd = infoDat.at(1);
										}
										if (infoDat.at(0) == "hidenamesbulletinboard")
										{
											hidenames = infoDat.at(1);
										}
									}
									if (publiccanadd != "" && hidenames != "")
									{
										int does_pubiccanadd = atoi(publiccanadd.c_str());
										int are_hidenames = atoi(hidenames.c_str());
										if ((does_pubiccanadd != 0 && does_pubiccanadd != 1) || (are_hidenames != 0 && are_hidenames != 1))
										{
											break;
										}
										ifstream ifff("save/bulletinboard/_" + pData->currentWorld + "/X" + coord + ".json");
										json j;
										ifff >> j;
										ifff.close();

										j["hidenames"] = are_hidenames;
										j["publiccanadd"] = does_pubiccanadd;

										ofstream of("save/bulletinboard/_" + pData->currentWorld + "/X" + coord + ".json");
										of << j << std::endl;
										of.close();
									}
								}
								catch (const std::out_of_range& e) {
									std::cout << e.what() << std::endl;
								}
							}
						}
						if (btn.substr(0, 19) == "bulletinboardclear_")
						{
							if (world == nullptr || pData->currentWorld == "EXIT" || serverIsFrozen) continue;
							string coord = btn.substr(btn.find("_") + 1);
							auto isdbox = std::experimental::filesystem::exists("save/bulletinboard/_" + pData->currentWorld + "/X" + coord + ".json");
							if (!isdbox)
							{
								Player::OnTalkBubble(peer, pData->netID, "`4An error occured while getting bulletin board info.", 0, true);
								continue;
							}
							if (pData->rawName == world->owner || world->owner == "" || mod(peer))
							{
								try {
									ifstream ifff("save/bulletinboard/_" + pData->currentWorld + "/X" + coord + ".json");
									json j;
									ifff >> j;
									ifff.close();
									if (j["inmail"] < 1)
									{
										Player::OnTalkBubble(peer, pData->netID, "`4There is no any mails!", 0, true);
										continue;
									}
									for (int i = 0; i < 90; i++)
									{
										j["mails"].at(i)["growid"] = "";
										j["mails"].at(i)["text"] = "";
									}
									j["inmail"] = 0;
									Player::OnTalkBubble(peer, pData->netID, "`2Bulletin Board emptied.", 0, true);

									ofstream of("save/bulletinboard/_" + pData->currentWorld + "/X" + coord + ".json");
									of << j << std::endl;
									of.close();
								}
								catch (const std::out_of_range& e) {
									std::cout << e.what() << std::endl;
								}
							}
						}
						if (btn.substr(0, 10) == "mailempty_")
						{
							if (world == nullptr || pData->currentWorld == "EXIT" || serverIsFrozen) continue;
							string coord = btn.substr(btn.find("_") + 1);
							auto isdbox = std::experimental::filesystem::exists("save/mailbox/_" + pData->currentWorld + "/X" + coord + ".json");
							if (!isdbox)
							{
								Player::OnTalkBubble(peer, pData->netID, "`4An error occured while getting mailbox info.", 0, true);
								continue;
							}
							if (pData->rawName == world->owner || world->owner == "" || mod(peer))
							{
								try {
									ifstream ifff("save/mailbox/_" + pData->currentWorld + "/X" + coord + ".json");
									json j;
									ifff >> j;
									ifff.close();
									if (j["inmail"] < 1)
									{
										Player::OnTalkBubble(peer, pData->netID, "`4There is no any mails!", 0, true);
										continue;
									}
									for (int i = 0; i < 20; i++)
									{
										j["mails"].at(i)["growid"] = "";
										j["mails"].at(i)["text"] = "";
									}
									j["inmail"] = 0;
									Player::OnTalkBubble(peer, pData->netID, "`2Mailbox emptied.", 0, true);

									int x = j["x"];
									int y = j["y"];
									for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
										if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
									}
									ofstream of("save/mailbox/_" + pData->currentWorld + "/X" + coord + ".json");
									of << j << std::endl;
									of.close();
								}
								catch (const std::out_of_range& e) {
									std::cout << e.what() << std::endl;
								}
							}
						}
						if (btn.substr(0, 14) == "bluemailempty_")
						{
							if (world == nullptr || pData->currentWorld == "EXIT" || serverIsFrozen) continue;
							string coord = btn.substr(btn.find("_") + 1);
							auto isdbox = std::experimental::filesystem::exists("save/bluemailbox/_" + pData->currentWorld + "/X" + coord + ".json");
							if (!isdbox)
							{
								Player::OnTalkBubble(peer, pData->netID, "`4An error occured while getting blue mailbox info.", 0, true);
								continue;
							}
							if (pData->rawName == world->owner || world->owner == "" || mod(peer))
							{
								try {
									ifstream ifff("save/bluemailbox/_" + pData->currentWorld + "/X" + coord + ".json");
									json j;
									ifff >> j;
									ifff.close();
									if (j["inmail"] < 1)
									{
										Player::OnTalkBubble(peer, pData->netID, "`4There is no any mails!", 0, true);
										continue;
									}
									for (int i = 0; i < 20; i++)
									{
										j["mails"].at(i)["growid"] = "";
										j["mails"].at(i)["text"] = "";
									}
									j["inmail"] = 0;
									Player::OnTalkBubble(peer, pData->netID, "`2Mailbox emptied.", 0, true);

									int x = j["x"];
									int y = j["y"];
									ENetPeer* currentPeer;
									for (currentPeer = server->peers;
										currentPeer < &server->peers[server->peerCount];
										++currentPeer)
									{
										if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL)
											continue;
									}
									ofstream of("save/bluemailbox/_" + pData->currentWorld + "/X" + coord + ".json");
									of << j << std::endl;
									of.close();
								}
								catch (const std::out_of_range& e) {
									std::cout << e.what() << std::endl;
								}
							}
						}
						if (btn.substr(0, 14) == "retrieveGifts_")
						{
							if (world == nullptr || pData->currentWorld == "EXIT" || serverIsFrozen) continue;
							string coord = btn.substr(btn.find("_") + 1);
							auto isdbox = std::experimental::filesystem::exists("save/donationboxes/_" + pData->currentWorld + "/X" + coord + ".json");
							if (!isdbox)
							{
								Player::OnTalkBubble(peer, pData->netID, "`4An error occured while getting donation box info.", 0, true);
								continue;
							}
							if (pData->rawName == world->owner || world->owner == "" || mod(peer))
							{
								try {
									ifstream ifff("save/donationboxes/_" + pData->currentWorld + "/X" + coord + ".json");
									json j;
									ifff >> j;
									ifff.close();
									if (j["donated"] < 1)
									{
										Player::OnTalkBubble(peer, pData->netID, "`4There is no any donations!", 0, true);
										continue;
									}
									bool success = true;
									for (int i = 0; i < 20; i++)
									{
										if (j["donatedItems"].at(i)["itemid"] != 0)
										{
											int howManyEarnings = j["donatedItems"].at(i)["itemcount"];
											int itemid = j["donatedItems"].at(i)["itemid"];
											string whoSent = j["donatedItems"].at(i)["sentBy"];
											if (j["donatedItems"].at(i)["itemcount"] > 250)
											{
												Player::OnTextOverlay(peer, "You cant receive that!");
												break;
											}
											if (CheckItemExists(peer, j["donatedItems"].at(i)["itemid"]))
											{
												int currentlyHave = GetQuantityOfItem(peer, j["donatedItems"].at(i)["itemid"]);
												if (j["donatedItems"].at(i)["itemcount"].get<double>() + currentlyHave < 251)
												{
													SaveItemMoreTimes(j["donatedItems"].at(i)["itemid"], j["donatedItems"].at(i)["itemcount"], peer, success, pData->rawName + " from donation box");
													j["donatedItems"].at(i)["itemid"] = 0;
													j["donatedItems"].at(i)["itemcount"] = 0;
													j["donatedItems"].at(i)["sentBy"] = "";
													j["donatedItems"].at(i)["note"] = "";
													j["donated"] = j["donated"].get<double>() - 1;
													ENetPeer* currentPeer;
													for (currentPeer = server->peers;
														currentPeer < &server->peers[server->peerCount];
														++currentPeer)
													{
														if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
														if (isHere(peer, currentPeer))
														{
															Player::OnConsoleMessage(currentPeer, "`w[`o" + pData->displayName + " `oreceives `5" + to_string(howManyEarnings) + " `w" + itemDefs[itemid].name + " `ofrom `w" + whoSent + "`o, how nice!`w]");
														}
													}
													Player::OnTalkBubble(peer, pData->netID, "`2Box emptied.", 0, true);
												}
												else
												{
													int kiekTrukstaIki250 = 250 - currentlyHave;
													SaveItemMoreTimes(j["donatedItems"].at(i)["itemid"], kiekTrukstaIki250, peer, success, pData->rawName + " from " + getItemDef(world->items.at(pData->lastPunchX + (pData->lastPunchY * world->width)).foreground).name + "");
													j["donatedItems"].at(i)["itemcount"] = j["donatedItems"].at(i)["itemcount"].get<double>() - kiekTrukstaIki250;
													howManyEarnings -= kiekTrukstaIki250;
													ENetPeer* currentPeer;
													for (currentPeer = server->peers;
														currentPeer < &server->peers[server->peerCount];
														++currentPeer)
													{
														if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
														if (isHere(peer, currentPeer))
														{
															Player::OnConsoleMessage(currentPeer, "`w[`o" + pData->rawName + " receives `5" + to_string(kiekTrukstaIki250) + " (`w" + to_string(howManyEarnings) + " `5left) `w" + itemDefs[itemid].name + " `ofrom `w" + whoSent + "`o, how nice!`w]");
														}
													}
													Player::OnTalkBubble(peer, pData->netID, "`2Box emptied.", 0, true);
												}
											}
											else
											{
												if (pData->inventory.items.size() == pData->currentInventorySize)
												{
													Player::OnTextOverlay(peer, "`4Your inventory is full! You can purchase an inventory upgrade in the shop.");
													break;
												}

												SaveItemMoreTimes(j["donatedItems"].at(i)["itemid"], j["donatedItems"].at(i)["itemcount"], peer, success, pData->rawName + " from donation box");
												j["donatedItems"].at(i)["itemid"] = 0;
												j["donatedItems"].at(i)["itemcount"] = 0;
												j["donatedItems"].at(i)["sentBy"] = "";
												j["donatedItems"].at(i)["note"] = "";
												j["donated"] = j["donated"].get<double>() - 1;
												ENetPeer* currentPeer;
												for (currentPeer = server->peers;
													currentPeer < &server->peers[server->peerCount];
													++currentPeer)
												{
													if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
													if (isHere(peer, currentPeer))
													{
														Player::OnConsoleMessage(currentPeer, "`w[`o" + pData->rawName + " receives `5" + to_string(howManyEarnings) + " `w" + itemDefs[itemid].name + " `ofrom `w" + whoSent + "`o, how nice!`w]");
													}
												}
												Player::OnTalkBubble(peer, pData->netID, "`2Box emptied.", 0, true);
											}
										}
									}

									if (j["donated"] <= 0)
									{
										ENetPeer* currentPeer;
										for (currentPeer = server->peers;
											currentPeer < &server->peers[server->peerCount];
											++currentPeer)
										{
											if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
										}
									}
									ofstream of("save/donationboxes/_" + pData->currentWorld + "/X" + coord + ".json");
									of << j << std::endl;
									of.close();
								}
								catch (const std::out_of_range& e) {
									std::cout << e.what() << std::endl;
								}
							}
						}
						if (btn.substr(0, 15) == "safeupdatepass_")
						{
							if (world == nullptr || pData->currentWorld == "EXIT" || serverIsFrozen) continue;
							string coord = btn.substr(btn.find("_") + 1);
							auto isdbox = std::experimental::filesystem::exists("save/safevault/_" + pData->currentWorld + "/X" + coord + ".json");
							if (!isdbox)
							{
								Player::OnTalkBubble(peer, pData->netID, "`4An error occured. Break the safe.", 0, true);
							}
							else
							{
								if (pData->rawName == PlayerDB::getProperName(world->owner) || world->owner == "" || mod(peer))
								{
									GTDialog mySafe;
									mySafe.addLabelWithIcon("`wSafe Vault", 8878, LABEL_BIG);
									ifstream ifff("save/safevault/_" + pData->currentWorld + "/X" + coord + ".json");
									json j;
									ifff >> j;
									if (j["password"] == "")
									{
										mySafe.addSmallText("The ingenious minds at GrowTech bring you the `2Safe Vault`` - a place to store your items safely with its integrated password option!");
										mySafe.addSmallText("How the password works:");
										mySafe.addSmallText("The Safe Vault requires both a `2password`` and a `2recovery answer`` to be entered to use a password.");
										mySafe.addSmallText("Enter your `2password`` and `2recovery answer`` below - keep them safe and `4DO NOT`` share them with anyone you do not trust!");
										mySafe.addSmallText("The password and recovery answer can be no longer than 12 characters in length - number and alphabet only please, no special characters are allowed!");
										mySafe.addSmallText("If you forget your password, enter your recovery answer to access the Safe Vault - The Safe Vault will `4NOT be password protected now``. You will need to enter a new password.");
										mySafe.addSmallText("You can change your password, however you will need to enter the old password before a new one can be used.");
										mySafe.addSmallText("`4WARNING``: DO NOT forget your password and recovery answer or you will not be able to access the Safe Vault!");
										mySafe.addSmallText("`4There is no password currently set on this Safe Vault.``");
										mySafe.addSmallText("Enter a new password.");
										mySafe.addInputBox("safenewpass_" + coord, "", "", 18);
										mySafe.addSmallText("Enter a recovery answer.");
										mySafe.addInputBox("safe_recoverypass", "", "", 12);
										mySafe.addButton("safe_setpassword", "Update Password");
										mySafe.addSpacer(SPACER_SMALL);
										mySafe.addQuickExit();
										mySafe.endDialog("Close", "", "Exit");
										Player::OnDialogRequest(peer, mySafe.finishDialog());
									}
									else
									{
										mySafe.addSmallText("The ingenious minds at GrowTech bring you the `2Safe Vault`` - a place to store your items safely with its integrated password option!");
										mySafe.addSmallText("How the password works:");
										mySafe.addSmallText("The Safe Vault requires both a `2password`` and a `2recovery answer`` to be entered to use a password.");
										mySafe.addSmallText("Enter your `2password`` and `2recovery answer`` below - keep them safe and `4DO NOT`` share them with anyone you do not trust!");
										mySafe.addSmallText("The password and recovery answer can be no longer than 12 characters in length - number and alphabet only please, no special characters are allowed!");
										mySafe.addSmallText("If you forget your password, enter your recovery answer to access the Safe Vault - The Safe Vault will `4NOT be password protected now``. You will need to enter a new password.");
										mySafe.addSmallText("You can change your password, however you will need to enter the old password before a new one can be used.");
										mySafe.addSmallText("`4WARNING``: DO NOT forget your password and recovery answer or you will not be able to access the Safe Vault!");
										mySafe.addSmallText("Enter your old password.");
										mySafe.addInputBox("safeoldpassword_" + coord, "", "", 18);
										mySafe.addSmallText("Enter a new password.");
										mySafe.addInputBox("safe_newpassword", "", "", 18);
										mySafe.addSmallText("Enter a recovery answer.");
										mySafe.addInputBox("storage_recoveryanswer", "", "", 12);
										mySafe.addButton("safe_updatepassword", "Update Password");
										mySafe.addSmallText("Remove the password protection from the `5Safe Vault``.");
										mySafe.addSmallText("You will need to enter the old password to remove the password protection.");
										mySafe.addButton("safe_removepassword", "Remove Password");
										mySafe.addSpacer(SPACER_SMALL);
										mySafe.addQuickExit();
										mySafe.endDialog("Close", "", "Exit");
										Player::OnDialogRequest(peer, mySafe.finishDialog());
									}
								}
							}
						}
						if (btn.substr(0, 21) == "boxlvl2DepositedItem_")
						{
							if (world == nullptr || pData->currentWorld == "EXIT" || serverIsFrozen) continue;
							if (pData->rawName == PlayerDB::getProperName(world->owner) || world->owner == "" || mod(peer))
							{
								try {
									string apos_and_coord = btn.erase(0, 21);
									string::size_type pos = apos_and_coord.find('_');
									string apos = "";
									string coord = "";

									if (pos != std::string::npos)
									{
										apos = apos_and_coord.substr(0, pos);
										apos_and_coord.erase(0, pos + 1);
										coord = apos_and_coord;
									}
									else
									{
										break;
									}
									auto isdbox = std::experimental::filesystem::exists("save/storageboxlvl2/_" + pData->currentWorld + "/X" + coord + ".json");
									if (!isdbox)
									{
										Player::OnTalkBubble(peer, pData->netID, "`4An error occured while getting box info.", 0, true);
										break;
									}

									bool contains_non_int3 = !std::regex_match(apos, std::regex("^[0-9]+$"));
									if (contains_non_int3 == true)
									{
										break;
									}

									ifstream ifff("save/storageboxlvl2/_" + pData->currentWorld + "/X" + coord + ".json");
									json j;
									ifff >> j;
									ifff.close();

									int itemid = 0;
									int count = 0;
									for (int i = 0; i < 40; i++)
									{
										if (j["storage"].at(i)["aposition"] == atoi(apos.c_str()))
										{
											itemid = j["storage"].at(i)["itemid"];
											count = j["storage"].at(i)["itemcount"];
											GTDialog storagelvl1;
											storagelvl1.addLabelWithIcon("`wStorage Box Xtreme - Level 2", 6288, LABEL_BIG);
											storagelvl1.addSmallText("`oYou have `w" + to_string(count) + " " + itemDefs[itemid].name + " `ostored.");
											storagelvl1.addSmallText("`oWithdraw how many?");
											storagelvl1.addInputBox("boxlvl2withdraw_" + coord + "_" + apos, "", to_string(count), 5);
											storagelvl1.addSpacer(SPACER_SMALL);
											storagelvl1.addButton("boxlvl2_withdrawConfirm", "Remove items");
											storagelvl1.addSpacer(SPACER_SMALL);
											storagelvl1.addQuickExit();
											storagelvl1.endDialog("Close", "", "Exit");
											Player::OnDialogRequest(peer, storagelvl1.finishDialog());
											break;
										}
									}
								}
								catch (const std::out_of_range& e) {
									std::cout << e.what() << std::endl;
								}
							}
						}
						if (btn.substr(0, 21) == "boxlvl3DepositedItem_")
						{
							if (world == nullptr || pData->currentWorld == "EXIT" || serverIsFrozen) continue;
							if (pData->rawName == PlayerDB::getProperName(world->owner) || world->owner == "" || mod(peer))
							{
								try {
									string apos_and_coord = btn.erase(0, 21);
									string::size_type pos = apos_and_coord.find('_');
									string apos = "";
									string coord = "";

									if (pos != std::string::npos)
									{
										apos = apos_and_coord.substr(0, pos);
										apos_and_coord.erase(0, pos + 1);
										coord = apos_and_coord;
									}
									else
									{
										break;
									}
									auto isdbox = std::experimental::filesystem::exists("save/storageboxlvl3/_" + pData->currentWorld + "/X" + coord + ".json");
									if (!isdbox)
									{
										Player::OnTalkBubble(peer, pData->netID, "`4An error occured while getting box info.", 0, true);
										break;
									}

									bool contains_non_int3 = !std::regex_match(apos, std::regex("^[0-9]+$"));
									if (contains_non_int3 == true)
									{
										break;
									}

									ifstream ifff("save/storageboxlvl3/_" + pData->currentWorld + "/X" + coord + ".json");
									json j;
									ifff >> j;
									ifff.close();

									int itemid = 0;
									int count = 0;
									for (int i = 0; i < 90; i++)
									{
										if (j["storage"].at(i)["aposition"] == atoi(apos.c_str()))
										{
											itemid = j["storage"].at(i)["itemid"];
											count = j["storage"].at(i)["itemcount"];
											GTDialog storagelvl1;
											storagelvl1.addLabelWithIcon("`wStorage Box Xtreme - Level 3", 6290, LABEL_BIG);
											storagelvl1.addSmallText("`oYou have `w" + to_string(count) + " " + itemDefs[itemid].name + " `ostored.");
											storagelvl1.addSmallText("`oWithdraw how many?");
											storagelvl1.addInputBox("boxlvl3withdraw_" + coord + "_" + apos, "", to_string(count), 5);
											storagelvl1.addSpacer(SPACER_SMALL);
											storagelvl1.addButton("boxlvl3_withdrawConfirm", "Remove items");
											storagelvl1.addSpacer(SPACER_SMALL);
											storagelvl1.addQuickExit();
											storagelvl1.endDialog("Close", "", "Exit");
											Player::OnDialogRequest(peer, storagelvl1.finishDialog());
											break;
										}
									}
								}
								catch (const std::out_of_range& e) {
									std::cout << e.what() << std::endl;
								}
							}
						}
						if (btn.substr(0, 21) == "boxlvl1DepositedItem_")
						{
							if (world == nullptr || pData->currentWorld == "EXIT" || serverIsFrozen) continue;
							if (pData->rawName == PlayerDB::getProperName(world->owner) || world->owner == "" || mod(peer))
							{
								try {
									string apos_and_coord = btn.erase(0, 21);
									string::size_type pos = apos_and_coord.find('_');
									string apos = "";
									string coord = "";

									if (pos != std::string::npos)
									{
										apos = apos_and_coord.substr(0, pos);
										apos_and_coord.erase(0, pos + 1);
										coord = apos_and_coord;
									}
									else
									{
										break;
									}
									auto isdbox = std::experimental::filesystem::exists("save/storageboxlvl1/_" + pData->currentWorld + "/X" + coord + ".json");
									if (!isdbox)
									{
										Player::OnTalkBubble(peer, pData->netID, "`4An error occured while getting box info.", 0, true);
										break;
									}

									bool contains_non_int3 = !std::regex_match(apos, std::regex("^[0-9]+$"));
									if (contains_non_int3 == true)
									{
										break;
									}

									ifstream ifff("save/storageboxlvl1/_" + pData->currentWorld + "/X" + coord + ".json");
									json j;
									ifff >> j;
									ifff.close();

									int itemid = 0;
									int count = 0;
									for (int i = 0; i < 20; i++)
									{
										if (j["storage"].at(i)["aposition"] == atoi(apos.c_str()))
										{
											itemid = j["storage"].at(i)["itemid"];
											count = j["storage"].at(i)["itemcount"];
											GTDialog storagelvl1;
											storagelvl1.addLabelWithIcon("`wStorage Box Xtreme - Level 1", 6286, LABEL_BIG);
											storagelvl1.addSmallText("`oYou have `w" + to_string(count) + " " + itemDefs[itemid].name + " `ostored.");
											storagelvl1.addSmallText("`oWithdraw how many?");
											storagelvl1.addInputBox("boxlvl1withdraw_" + coord + "_" + apos, "", to_string(count), 5);
											storagelvl1.addSpacer(SPACER_SMALL);
											storagelvl1.addButton("boxlvl1_withdrawConfirm", "Remove items");
											storagelvl1.addSpacer(SPACER_SMALL);
											storagelvl1.addQuickExit();
											storagelvl1.endDialog("Close", "", "Exit");
											Player::OnDialogRequest(peer, storagelvl1.finishDialog());
											break;
										}
									}
								}
								catch (const std::out_of_range& e) {
									std::cout << e.what() << std::endl;
								}
							}
						}
						// store btn
						if (btn == "getKey")
						{
							if (isWorldOwner(peer, world) || admin(peer))
							{
								try
								{
									auto iscontains = false;
									SearchInventoryItem(peer, 1424, 1, iscontains);
									if (!iscontains) {
										if (world->droppedCount > 0)
										{
											bool found = false;
											for (int i = 0; i < world->droppedItems.size(); i++)
											{
												int x_p = world->droppedItems.at(i).x / 32;
												int y_p = world->droppedItems.at(i).y / 32;
												if (world->items.at(x_p + (static_cast<std::vector<WorldItem, std::allocator<WorldItem>>::size_type>(y_p) * world->width)).foreground == 0)
												{
													found = true;
													break;
												}
											}
											if (found) {
												Player::OnTalkBubble(peer, pData->netID, "`4Oops!... Can't trade a world with floating items that are unblocked or in treasure chests!``", 0, false);
												continue;
											}
										}
										Player::OnTalkBubble(peer, pData->netID, "You got a `#World Key``! You can now trade this world to other players.", 0, false);
										Player::PlayAudio(peer, "audio/use_lock.wav", 0);
										bool success = true;
										SaveItemMoreTimes(1424, 1, peer, success);
									}
								}
								catch (const std::out_of_range& e) {
									std::cout << e.what() << std::endl;
								}
							}
						}
						if (btn == "trade") {
							if (static_cast<PlayerInfo*>(peer->data)->isCursed == true) {
								Player::OnConsoleMessage(peer, "`4You are cursed now!");
								continue;
							}
							string trade_name = pData->lastInfo;
							if (trade_name.size() < 3) {
								Player::OnConsoleMessage(peer, "You'll need to enter at least the first three characters of the person's name.");
								continue;
							}
							bool Found = false, Block = false;
							int Same_name = 0, Sub_worlds_name = 0;
							string Intel_sense_nick = "";
							for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
								if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
								if (isHere(peer, currentPeer)) {
									if (getStrLower(static_cast<PlayerInfo*>(currentPeer->data)->displayName).find(getStrLower(trade_name)) != string::npos) Same_name++;
								}
								else if (mod(peer)) {
									if (getStrLower(static_cast<PlayerInfo*>(currentPeer->data)->displayName).find(getStrLower(trade_name)) != string::npos) Sub_worlds_name++;
								}
							}
							if (Same_name > 1) {
								Player::OnConsoleMessage(peer, "`oThere are more than two players in this world starting with " + trade_name + " `obe more specific!");
								continue;
							}
							for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
								if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
								if (isHere(peer, currentPeer) && getStrLower(static_cast<PlayerInfo*>(currentPeer->data)->displayName).find(getStrLower(trade_name)) != string::npos) {
									if (static_cast<PlayerInfo*>(currentPeer->data)->rawName == static_cast<PlayerInfo*>(peer->data)->rawName) {
										Player::OnConsoleMessage(peer, "`oYou trade all your stuff to yourself in exchange for all your stuff.");
										Block = true;
										break;
									}
									Block = true;
									Found = true;
									Intel_sense_nick = static_cast<PlayerInfo*>(currentPeer->data)->displayName;
									if (static_cast<PlayerInfo*>(currentPeer->data)->trade) {
										Player::OnTalkBubble(peer, static_cast<PlayerInfo*>(peer->data)->netID, "`wThat person is busy.", 0, false);
										break;
									}
									static_cast<PlayerInfo*>(peer->data)->trade = false;
									static_cast<PlayerInfo*>(peer->data)->trade_netid = static_cast<PlayerInfo*>(currentPeer->data)->netID;
									Player::OnStartTrade(peer, static_cast<PlayerInfo*>(currentPeer->data)->displayName, static_cast<PlayerInfo*>(currentPeer->data)->netID);
									break;
								}
							}
							if (Block) continue;
							if (!Found) {
								Player::OnConsoleMessage(peer, "`4Oops:`` There is nobody currently in this world with a name starting with `w" + trade_name + "``.");
							}
						}
						if (btn == "rift_cape_edit") {
							string aura_on = "0", portal_aura1 = "0", starfield_aura1 = "0", electrical_aura1 = "0", portal_aura2 = "0", starfield_aura2 = "0", electrical_aura2 = "0";
							switch (pData->rift_type) { /*1style*/
							case 2555: /*portal*/
							{
								portal_aura1 = "1";
								aura_on = "1";
								break;
							}
							case 2811: /*starfield*/
							{
								starfield_aura1 = "1";
								aura_on = "1";
								break;
							}
							case 3067: /*electrical*/
							{
								electrical_aura1 = "1";
								aura_on = "1";
								break;
							}
							default:
							{
								break;
							}
							}
							Player::OnDialogRequest(peer, "set_default_color|`o\nadd_label_with_icon|big|`wRift Cape``|left|10424|\nadd_spacer|small|\nadd_text_input|text_input_time_cycle|Time Dilation Cycle Time:|30|5|\nadd_checkbox|checkbox_time_cycle|Time Dilation On / Off|0\nadd_button|button_manual|Instructions|noflags|0|0|\nadd_spacer|small|\nadd_label|big|Cape Style 1|left\nadd_spacer|small|\nadd_label|small|Cape Color:|left\nadd_text_input|text_input_cape_color0|Cape - R,G,B:|" + to_string(pData->rift_cape_r) + "," + to_string(pData->rift_cape_g) + "," + to_string(pData->rift_cape_b) + "|11|\nadd_spacer|small|\nadd_checkbox|checkbox_cape_collar0|Cape Collar On / Off|1\nadd_label|small|Cape Collar Color:|left\nadd_text_input|text_input_collar_color0|Collar - R,G,B:|" + to_string(pData->rift_collar_r) + "," + to_string(pData->rift_collar_g) + "," + to_string(pData->rift_collar_b) + "|11|\nadd_spacer|small|\nadd_checkbox|checkbox_closed_cape0|Closed Cape|0\nadd_checkbox|checkbox_open_on_move0|Open Cape on Movement|1\nadd_checkbox|checkbox_aura0|Aura On / Off|" + aura_on + "\nadd_checkbox|checkbox_aura_1st0|      Portal Aura|" + portal_aura1 + "\nadd_checkbox|checkbox_aura_2nd0|      Starfield Aura|" + starfield_aura1 + "\nadd_checkbox|checkbox_aura_3rd0|      Electrical Aura|" + electrical_aura1 + "\nadd_label|big|Cape Style 2|left\nadd_spacer|small|\nadd_label|small|Cape Color:|left\nadd_text_input|text_input_cape_color1|Cape - R,G,B:|137,30,43|11|\nadd_spacer|small|\nadd_checkbox|checkbox_cape_collar1|Cape Collar On / Off|1\nadd_label|small|Cape Collar Color:|left\nadd_text_input|text_input_collar_color1|Collar - R,G,B:|34,35,63|11|\nadd_spacer|small|\nadd_checkbox|checkbox_closed_cape1|Closed Cape|1\nadd_checkbox|checkbox_open_on_move1|Open Cape on Movement|1\nadd_checkbox|checkbox_aura1|Aura On / Off|1\nadd_checkbox|checkbox_aura_1st1|      Portal Aura|" + portal_aura2 + "\nadd_checkbox|checkbox_aura_2nd1|      Starfield Aura|" + starfield_aura2 + "\nadd_checkbox|checkbox_aura_3rd1|      Electrical Aura|" + electrical_aura2 + "\nadd_spacer|small|\nadd_button|restore_default|Restore to Default|noflags|0|0|\nend_dialog|dialog_rift_cape|Cancel|Update|\nadd_quick_exit|");
						}
						if (btn == "goals") {
							int firefighterxp = 1500;
							if (pData->firefighterlevel > 0) firefighterxp = firefighterxp * pData->firefighterlevel;
							if (pData->firefighterlevel == 0) firefighterxp = 750;
							int providerxp = 1300;
							if (pData->providerlevel > 0) providerxp = providerxp * pData->providerlevel;
							if (pData->providerlevel == 0) providerxp = 600;
							int geigerxp = 600;
							if (pData->geigerlevel > 0) geigerxp = geigerxp * pData->geigerlevel;
							if (pData->geigerlevel == 0) geigerxp = 350;
							int fishermanxp = 1600;
							if (pData->fishermanlevel > 0) fishermanxp = fishermanxp * pData->fishermanlevel;
							if (pData->fishermanlevel == 0) fishermanxp = 900;
							Player::OnDialogRequest(peer, "add_label_with_icon|big|`wRole Stats``|left|982|\nadd_spacer|small|\nadd_textbox|`9What prizes and powers have you unlocked in your Roles and what's left to discover? Find out here!``|left|\nadd_spacer|small|\nadd_player_info|Firefighter|" + to_string(pData->firefighterlevel) + "|" + to_string(pData->firefighterxp) + "|" + to_string(firefighterxp) + "|\nadd_spacer|small|\nadd_button|viewFirefighter|`0View Firefighter Rewards``|noflags|0|0|\nadd_spacer|small|\nadd_player_info|Provider|" + to_string(pData->providerlevel) + "|" + to_string(pData->providerxp) + "|" + to_string(providerxp) + "|\nadd_spacer|small|\nadd_button|viewProvider|`0View Provider Rewards``|noflags|0|0|\nadd_spacer|small|\nadd_player_info|Geiger Hunter|" + to_string(pData->geigerlevel) + "|" + to_string(pData->geigerxp) + "|" + to_string(geigerxp) + "|\nadd_spacer|small|\nadd_button|viewGeiger|`0View Geiger Hunter Rewards``|noflags|0|0|\nadd_spacer|small|\nadd_player_info|Fisherman|" + to_string(pData->fishermanlevel) + "|" + to_string(pData->fishermanxp) + "|" + to_string(fishermanxp) + "|\nadd_spacer|small|\nadd_button|viewFisherman|`0View Fisherman Rewards``|noflags|0|0|\nadd_spacer|small|\nadd_button|back|Back|noflags|0|0|\nend_dialog|mainwrenchpage|||\nadd_quick_exit|");
						}
						if (btn == "viewFisherman") {
							string level1 = "", level2 = "", level3 = "", level4 = "", level5 = "", level6 = "", level7 = "", level8 = "", level9 = "", level10 = "";
							if (pData->fishermanlevel >= 1) {
								level1 = "`oUnlocked: Licorice Rod``";
							}
							else {
								level1 = "`a(Locked) Licorice Rod``";
							}
							if (pData->fishermanlevel >= 2) {
								level2 = "`oUnlocked: Quantum: 1% chances of getting items as a bonus drop from from fishing``";
							}
							else {
								level2 = "`a(Locked) Quantum: 1% chances of getting items as a bonus drop from from fishing``";
							}
							if (pData->fishermanlevel >= 3) {
								level3 = "`oUnlocked: Adds a chance to get Golden Block as a bonus drop from fishing``";
							}
							else {
								level3 = "`a(Locked) Adds a chance to get Golden Block as a bonus drop from fishing``";
							}
							if (pData->fishermanlevel >= 4) {
								level4 = "`oUnlocked: Floating Leaf``|left|3074|\nadd_label_with_icon|sml|`oUnlocked: Quantum (level 2) - Chance increase to 2%``";
							}
							else {
								level4 = "`a(Locked) Floating Leaf``|left|3074|\nadd_label_with_icon|sml|`a(Locked) Quantum (level 2) - Chance increase to 2%``";
							}
							if (pData->fishermanlevel >= 5) {
								level5 = "`oUnlocked: Magical Rainbow Fishing Rod``";
							}
							else {
								level5 = "`a(Locked) Magical Rainbow Fishing Rod``";
							}
							if (pData->fishermanlevel >= 6) {
								level6 = "`oUnlocked: Quantum (level 3) - Chance increase to 3%``";
							}
							else {
								level6 = "`a(Locked) Quantum (level 3) - Chance increase to 3%``";
							}
							if (pData->fishermanlevel >= 7) {
								level7 = "`oUnlocked: Bubble Wings``";
							}
							else {
								level7 = "`a(Locked) Bubble Wings``";
							}
							if (pData->fishermanlevel >= 8) {
								level8 = "`oUnlocked: Quantum (level 4) - Chance increase to 4%``|left|7002|\nadd_label_with_icon|sml|`oUnlocked: Cursed Fishing Rod``|left|3100|";
							}
							else {
								level8 = "`a(Locked) Quantum (level 4) - Chance increase to 4%``|left|7002|\nadd_label_with_icon|sml|`a(Locked) Cursed Fishing Rod``|left|3100|";
							}
							if (pData->fishermanlevel >= 9) {
								level9 = "`oUnlocked: Ancestral Tesseract of Dimensions``";
							}
							else {
								level9 = "`a(Locked) Ancestral Tesseract of Dimensions``";
							}
							if (pData->fishermanlevel >= 10) {
								level10 = "`oUnlocked: Adds a chance to get double gems from fishing``|left|112|\nadd_label_with_icon|sml|`oUnlocked: Quantum (level 5) - Chance increase to 5%``|left|7002|\nadd_label_with_icon|sml|`oUnlocked: Goldenrod``";
							}
							else {
								level10 = "`a(Locked) Adds a chance to get double gems from fishing``|left|112|\nadd_label_with_icon|sml|`a(Locked) Quantum (level 5) - Chance increase to 5%``|left|7002|\nadd_label_with_icon|sml|`a(Locked) Goldenrod``";
							}
							Player::OnDialogRequest(peer, "add_label_with_icon|big|`wFisherman Rewards``|left|10262|\nadd_spacer|small|\nadd_textbox|`9Here are all the Fisherman rewards that you have earned so far!``|left|\nadd_spacer|small|\nadd_spacer|small|\nadd_smalltext|Level 1 rewards:|left|\nadd_label_with_icon|sml|" + level1 + "|left|3010|\nadd_spacer|small|\nadd_smalltext|Level 2 rewards:|left|\nadd_label_with_icon|sml|" + level2 + "|left|7002|\nadd_spacer|small|\nadd_smalltext|Level 3 rewards:|left|\nadd_label_with_icon|sml|" + level3 + "|left|260|\nadd_spacer|small|\nadd_smalltext|Level 4 rewards:|left|\nadd_label_with_icon|sml|" + level4 + "|left|7002|\nadd_spacer|small|\nadd_smalltext|Level 5 rewards:|left|\nadd_label_with_icon|sml|" + level5 + "|left|5740|\nadd_spacer|small|\nadd_smalltext|Level 6 rewards:|left|\nadd_label_with_icon|sml|" + level6 + "|left|7002|\nadd_spacer|small|\nadd_smalltext|Level 7 rewards:|left|\nadd_label_with_icon|sml|" + level7 + "|left|1550|\nadd_spacer|small|\nadd_smalltext|Level 8 rewards:|left|\nadd_label_with_icon|sml|" + level8 + "|left|10424|\nadd_spacer|small|\nadd_smalltext|Level 9 rewards:|left|\nadd_label_with_icon|sml|" + level9 + "|left|5080|\nadd_spacer|small|\nadd_smalltext|Level 10 rewards:|left|\nadd_label_with_icon|sml|" + level10 + "|left|3040|\nadd_spacer|small|\nadd_button|back|Back|noflags|0|0|\nend_dialog|roleRewardsPage|||\nadd_quick_exit|");
						}
						if (btn == "viewGeiger") {
							string level1 = "", level2 = "", level3 = "", level4 = "", level5 = "", level6 = "", level7 = "", level8 = "", level9 = "", level10 = "";
							if (pData->geigerlevel >= 1) {
								level1 = "`oUnlocked: Uranium Necklace``";
							}
							else {
								level1 = "`a(Locked) Uranium Necklace``";
							}
							if (pData->geigerlevel >= 2) {
								level2 = "`oUnlocked: Infusion: 1% chances of not gaining irradiated mod``";
							}
							else {
								level2 = "`a(Locked) Infusion: 1% chances of not gaining irradiated mod``";
							}
							if (pData->geigerlevel >= 3) {
								level3 = "`oUnlocked: Adds a chance to get Uranium Block as a bonus drop from geiger hunting``";
							}
							else {
								level3 = "`a(Locked) Adds a chance to get Uranium Block as a bonus drop from geiger hunting``";
							}
							if (pData->geigerlevel >= 4) {
								level4 = "`oUnlocked: Weil Magic: 1% chances of gaining extra bonus drop from geiger hunting``|left|3764|\nadd_label_with_icon|sml|`oUnlocked: Infusion (level 2) - Chance increase to 2%``";
							}
							else {
								level4 = "`a(Locked) Weil Magic: 1% chances of gaining extra bonus drop from geiger hunting``|left|3764|\nadd_label_with_icon|sml|`a(Locked) Infusion (level 2) - Chance increase to 2%``";
							}
							if (pData->geigerlevel >= 5) {
								level5 = "`oUnlocked: Adds a chance to get Electrical Power Cube as a bonus drop from geiger hunting``";
							}
							else {
								level5 = "`a(Locked) Adds a chance to get Electrical Power Cube as a bonus drop from geiger hunting``";
							}
							if (pData->geigerlevel >= 6) {
								level6 = "`oUnlocked: Infusion (level 3) - Chance increase to 3%``";
							}
							else {
								level6 = "`a(Locked) Infusion (level 3) - Chance increase to 3%``";
							}
							if (pData->geigerlevel >= 7) {
								level7 = "`oUnlocked: Unlocked: Weil Magic (level 2) - Chance increase to 2%``";
							}
							else {
								level7 = "`a(Locked) Unlocked: Weil Magic (level 2) - Chance increase to 2%``";
							}
							if (pData->geigerlevel >= 8) {
								level8 = "`oUnlocked: Infusion (level 4) - Chance increase to 4%``|left|9386|\nadd_label_with_icon|sml|`oUnlocked: Rift Cape``|left|10424|";
							}
							else {
								level8 = "`a(Locked) Infusion (level 4) - Chance increase to 4%``|left|9386|\nadd_label_with_icon|sml|`a(Locked) Rift Cape``|left|10424|";
							}
							if (pData->geigerlevel >= 9) {
								level9 = "`oUnlocked: Ancestral Lens of Riches``";
							}
							else {
								level9 = "`a(Locked) Ancestral Lens of Riches``";
							}
							if (pData->geigerlevel >= 10) {
								level10 = "`oUnlocked: Adds a chance to get Growtoken as a bonus drop from geiger hunting``|left|1486|\nadd_label_with_icon|sml|`oUnlocked: Infusion (level 5) - Chance increase to 5%``|left|9386|\nadd_label_with_icon|sml|`oUnlocked: Unique Prize``";
							}
							else {
								level10 = "`a(Locked) Adds a chance to get Growtoken as a bonus drop from geiger hunting``|left|1486|\nadd_label_with_icon|sml|`a(Locked) Infusion (level 5) - Chance increase to 5%``|left|9386|\nadd_label_with_icon|sml|`a(Locked) Unique Prize``";
							}
							Player::OnDialogRequest(peer, "add_label_with_icon|big|`wGeiger Hunter Rewards``|left|2204|\nadd_spacer|small|\nadd_textbox|`9Here are all the Geiger Hunter rewards that you have earned so far!``|left|\nadd_spacer|small|\nadd_spacer|small|\nadd_smalltext|Level 1 rewards:|left|\nadd_label_with_icon|sml|" + level1 + "|left|4656|\nadd_spacer|small|\nadd_smalltext|Level 2 rewards:|left|\nadd_label_with_icon|sml|" + level2 + "|left|9386|\nadd_spacer|small|\nadd_smalltext|Level 3 rewards:|left|\nadd_label_with_icon|sml|" + level3 + "|left|4658|\nadd_spacer|small|\nadd_smalltext|Level 4 rewards:|left|\nadd_label_with_icon|sml|" + level4 + "|left|9386|\nadd_spacer|small|\nadd_smalltext|Level 5 rewards:|left|\nadd_label_with_icon|sml|" + level5 + "|left|6976|\nadd_spacer|small|\nadd_smalltext|Level 6 rewards:|left|\nadd_label_with_icon|sml|" + level6 + "|left|9386|\nadd_spacer|small|\nadd_smalltext|Level 7 rewards:|left|\nadd_label_with_icon|sml|" + level7 + "|left|3764|\nadd_spacer|small|\nadd_smalltext|Level 8 rewards:|left|\nadd_label_with_icon|sml|" + level8 + "|left|10424|\nadd_spacer|small|\nadd_smalltext|Level 9 rewards:|left|\nadd_label_with_icon|sml|" + level9 + "|left|5084|\nadd_spacer|small|\nadd_smalltext|Level 10 rewards:|left|\nadd_label_with_icon|sml|" + level10 + "|left|2478|\nadd_spacer|small|\nadd_button|back|Back|noflags|0|0|\nend_dialog|roleRewardsPage|||\nadd_quick_exit|");
						}
						if (btn == "viewProvider") {
							string level1 = "", level2 = "", level3 = "", level4 = "", level5 = "", level6 = "", level7 = "", level8 = "", level9 = "", level10 = "";
							if (pData->providerlevel >= 1) {
								level1 = "`oUnlocked: Awkward Friendly Unicorn``";
							}
							else {
								level1 = "`a(Locked) Awkward Friendly Unicorn``";
							}
							if (pData->providerlevel >= 2) {
								level2 = "`oUnlocked: Weed Magic: 1% chances of providers dropping double items``";
							}
							else {
								level2 = "`a(Locked) Weed Magic: 1% chances of providers dropping double items``";
							}
							if (pData->providerlevel >= 3) {
								level3 = "`oUnlocked: 3 Growtokens``";
							}
							else {
								level3 = "`a(Locked) 3 Growtokens``";
							}
							if (pData->providerlevel >= 4) {
								level4 = "`oUnlocked: Adds a chance to get Smaraged Block as a bonus drop from harvesting providers``|left|5136|\nadd_label_with_icon|sml|`oUnlocked: Weed Magic (level 2) - Chance increase to 2%``";
							}
							else {
								level4 = "`a(Locked) Adds a chance to get Smaraged Block as a bonus drop from harvesting providers``|left|5136|\nadd_label_with_icon|sml|`a(Locked) Weed Magic (level 2) - Chance increase to 2%``";
							}
							if (pData->providerlevel >= 5) {
								level5 = "`oUnlocked: Ancestral Seed of Life``";
							}
							else {
								level5 = "`a(Locked) Ancestral Seed of Life``";
							}
							if (pData->providerlevel >= 6) {
								level6 = "`oUnlocked: Weed Magic (level 3) - Chance increase to 3%``";
							}
							else {
								level6 = "`a(Locked) Weed Magic (level 3) - Chance increase to 3%``";
							}
							if (pData->providerlevel >= 7) {
								level7 = "`oUnlocked: Adds a chance to get Emerald Shard as a bonus drop from harvesting providers``";
							}
							else {
								level7 = "`a(Locked) Adds a chance to get Emerald Shard as a bonus drop from harvesting providers``";
							}
							if (pData->providerlevel >= 8) {
								level8 = "`oUnlocked: 30-Day Premium Subscription Token``|left|6860|\nadd_label_with_icon|sml|`oUnlocked: Weed Magic (level 4) - Chance increase to 4%``";
							}
							else {
								level8 = "`a(Locked) 30-Day Premium Subscription Token``|left|6860|\nadd_label_with_icon|sml|`a(Locked) Weed Magic (level 4) - Chance increase to 4%``";
							}
							if (pData->providerlevel >= 9) {
								level9 = "`oUnlocked: 10 Growtokens``";
							}
							else {
								level9 = "`a(Locked) 10 Growtokens``";
							}
							if (pData->providerlevel >= 10) {
								level10 = "`oUnlocked: Adds a chance for providers to drop themself without losing one``|left|10072|\nadd_label_with_icon|sml|`oUnlocked: Weed Magic (level 5) - Chance increase to 5%``|left|954|\nadd_label_with_icon|sml|`oUnlocked: Unique Prize``";
							}
							else {
								level10 = "`a(Locked) Adds a chance for providers to drop themself without losing one``|left|10072|\nadd_label_with_icon|sml|`a(Locked) Weed Magic (level 5) - Chance increase to 5%``|left|954|\nadd_label_with_icon|sml|`a(Locked) Unique Prize``";
							}
							Player::OnDialogRequest(peer, "add_label_with_icon|big|`wProvider Rewards``|left|872|\nadd_spacer|small|\nadd_textbox|`9Here are all the Provider rewards that you have earned so far!``|left|\nadd_spacer|small|\nadd_spacer|small|\nadd_smalltext|Level 1 rewards:|left|\nadd_label_with_icon|sml|" + level1 + "|left|1636|\nadd_spacer|small|\nadd_smalltext|Level 2 rewards:|left|\nadd_label_with_icon|sml|" + level2 + "|left|954|\nadd_spacer|small|\nadd_smalltext|Level 3 rewards:|left|\nadd_label_with_icon|sml|" + level3 + "|left|1486|\nadd_spacer|small|\nadd_smalltext|Level 4 rewards:|left|\nadd_label_with_icon|sml|" + level4 + "|left|954|\nadd_spacer|small|\nadd_smalltext|Level 5 rewards:|left|\nadd_label_with_icon|sml|" + level5 + "|left|5082|\nadd_spacer|small|\nadd_smalltext|Level 6 rewards:|left|\nadd_label_with_icon|sml|" + level6 + "|left|954|\nadd_spacer|small|\nadd_smalltext|Level 7 rewards:|left|\nadd_label_with_icon|sml|" + level7 + "|left|2410|\nadd_spacer|small|\nadd_smalltext|Level 8 rewards:|left|\nadd_label_with_icon|sml|" + level8 + "|left|954|\nadd_spacer|small|\nadd_smalltext|Level 9 rewards:|left|\nadd_label_with_icon|sml|" + level9 + "|left|1486|\nadd_spacer|small|\nadd_smalltext|Level 10 rewards:|left|\nadd_label_with_icon|sml|" + level10 + "|left|2478|\nadd_spacer|small|\nadd_button|back|Back|noflags|0|0|\nend_dialog|roleRewardsPage|||\nadd_quick_exit|");
						}
						if (btn == "viewFirefighter") {
							string level1 = "", level2 = "", level3 = "", level4 = "", level5 = "", level6 = "", level7 = "", level8 = "", level9 = "", level10 = "";
							if (pData->firefighterlevel >= 1) {
								level1 = "`oUnlocked: Growtoken``";
							}
							else {
								level1 = "`a(Locked) Growtoken``";
							}
							if (pData->firefighterlevel >= 2) {
								level2 = "`oUnlocked: Fire King: 1% chances of receiving bonus gems when putting out fire``";
							}
							else {
								level2 = "`a(Locked) Fire King: 1% chances of receiving bonus gems when putting out fire``";
							}
							if (pData->firefighterlevel >= 3) {
								level3 = "`oUnlocked: Adds a chance to get Amethyst Block as a bonus drop from putting out fire``";
							}
							else {
								level3 = "`a(Locked) Adds a chance to get Amethyst Block as a bonus drop from putting out fire``";
							}
							if (pData->firefighterlevel >= 4) {
								level4 = "`oUnlocked: Adds a chance to get Fallen Pillar as a bonus drop from putting out fire``|left|7156|\nadd_label_with_icon|sml|`oUnlocked: Fire King (level 2) - Chance increase to 2%``";
							}
							else {
								level4 = "`a(Locked) Adds a chance to get Fallen Pillar as a bonus drop from putting out fire``|left|7156|\nadd_label_with_icon|sml|`a(Locked) Fire King (level 2) - Chance increase to 2%``";
							}
							if (pData->firefighterlevel >= 5) {
								level5 = "`oUnlocked: Burning Hands``";
							}
							else {
								level5 = "`a(Locked) Burning Hands``";
							}
							if (pData->firefighterlevel >= 6) {
								level6 = "`oUnlocked: Fire King (level 3) - Chance increase to 3%``";
							}
							else {
								level6 = "`a(Locked) Fire King (level 3) - Chance increase to 3%``";
							}
							if (pData->firefighterlevel >= 7) {
								level7 = "`oUnlocked: Adds a chance to get Diamond Stone as a bonus drop from putting out fire``";
							}
							else {
								level7 = "`a(Locked) Adds a chance to get Diamond Stone as a bonus drop from putting out fire``";
							}
							if (pData->firefighterlevel >= 8) {
								level8 = "`oUnlocked: 30-Day Premium Subscription Token``|left|6860|\nadd_label_with_icon|sml|`oUnlocked: Fire King (level 4) - Chance increase to 4%``";
							}
							else {
								level8 = "`a(Locked) 30-Day Premium Subscription Token``|left|6860|\nadd_label_with_icon|sml|`a(Locked) Fire King (level 4) - Chance increase to 4%``";
							}
							if (pData->firefighterlevel >= 9) {
								level9 = "`oUnlocked: 10 Growtokens``";
							}
							else {
								level9 = "`a(Locked) 10 Growtokens``";
							}
							if (pData->firefighterlevel >= 10) {
								level10 = "`oUnlocked: Adds a chance to get foreground block that was on fire as a bonus drop``|left|10072|\nadd_label_with_icon|sml|`oUnlocked: Fire King (level 5) - Chance increase to 5%``|left|3046|\nadd_label_with_icon|sml|`oUnlocked: Unique Prize``";
							}
							else {
								level10 = "`a(Locked) Adds a chance to get foreground block that was on fire as a bonus drop``|left|10072|\nadd_label_with_icon|sml|`a(Locked) Fire King (level 5) - Chance increase to 5%``|left|3046|\nadd_label_with_icon|sml|`a(Locked) Unique Prize``";
							}
							Player::OnDialogRequest(peer, "add_label_with_icon|big|`wFirefighter Rewards``|left|3046|\nadd_spacer|small|\nadd_textbox|`9Here are all the Firefighter rewards that you have earned so far!``|left|\nadd_spacer|small|\nadd_spacer|small|\nadd_smalltext|Level 1 rewards:|left|\nadd_label_with_icon|sml|" + level1 + "|left|1486|\nadd_spacer|small|\nadd_smalltext|Level 2 rewards:|left|\nadd_label_with_icon|sml|" + level2 + "|left|3046|\nadd_spacer|small|\nadd_smalltext|Level 3 rewards:|left|\nadd_label_with_icon|sml|" + level3 + "|left|4762|\nadd_spacer|small|\nadd_smalltext|Level 4 rewards:|left|\nadd_label_with_icon|sml|" + level4 + "|left|3046|\nadd_spacer|small|\nadd_smalltext|Level 5 rewards:|left|\nadd_label_with_icon|sml|" + level5 + "|left|4996|\nadd_spacer|small|\nadd_smalltext|Level 6 rewards:|left|\nadd_label_with_icon|sml|" + level6 + "|left|3046|\nadd_spacer|small|\nadd_smalltext|Level 7 rewards:|left|\nadd_label_with_icon|sml|" + level7 + "|left|5138|\nadd_spacer|small|\nadd_smalltext|Level 8 rewards:|left|\nadd_label_with_icon|sml|" + level8 + "|left|3046|\nadd_spacer|small|\nadd_smalltext|Level 9 rewards:|left|\nadd_label_with_icon|sml|" + level9 + "|left|1486|\nadd_spacer|small|\nadd_smalltext|Level 10 rewards:|left|\nadd_label_with_icon|sml|" + level10 + "|left|2478|\nadd_spacer|small|\nadd_button|back|Back|noflags|0|0|\nend_dialog|roleRewardsPage|||\nadd_quick_exit|");
						}
						if (btn == "pullstock")
						{
							if (world->owner != "" && !isWorldOwner(peer, world) && !mod(peer)) continue;
							int xxx = pData->lastPunchX;
							int yyy = pData->lastPunchY;
							if (xxx < 0 && yyy < 0) continue;
							try {
								int withdrawcpy = world->items.at(xxx + (static_cast<std::vector<WorldItem, std::allocator<WorldItem>>::size_type>(yyy) * world->width)).vdraw;
								if (withdrawcpy != 0) {
									Player::OnTalkBubble(peer, pData->netID, "`wWithdraw the world locks first!", 0, true);
									continue;
								}
								int realid = world->items.at(xxx + (static_cast<std::vector<WorldItem, std::allocator<WorldItem>>::size_type>(yyy) * world->width)).vid;
								int countid = world->items.at(xxx + (static_cast<std::vector<WorldItem, std::allocator<WorldItem>>::size_type>(yyy) * world->width)).vcount;
								if (CheckItemMaxed(peer, realid, countid)) {
									Player::OnTalkBubble(peer, pData->netID, "`wYou can't carry any of those!", 0, true);
									Player::OnConsoleMessage(peer, "`oYou can't carry any of those!");
									continue;
								}
								if (countid > 200) {
									bool success = true;
									SaveItemMoreTimes(realid, 200, peer, success, pData->rawName + " collected from Vending Machine");
									world->items.at(xxx + (yyy * world->width)).vcount -= 200;
									Player::OnTalkBubble(peer, pData->netID, "`wYou picked up 200 " + getItemDef(realid).name + "`w, leaving " + to_string(world->items.at(xxx + (yyy * world->width)).vcount) + " in the machine.", 0, true);
									Player::OnConsoleMessage(peer, "`oYou picked up 200 " + getItemDef(realid).name + "`o, leaving " + to_string(world->items.at(xxx + (yyy * world->width)).vcount) + " in the machine.");
									ENetPeer* currentPeer;
									for (currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
										if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
										if (isHere(peer, currentPeer)) {
											if (world->items.at(xxx + (yyy * world->width)).opened && world->items.at(xxx + (yyy * world->width)).vcount < world->items.at(xxx + (yyy * world->width)).vprice) {
												UpdateVend(currentPeer, xxx, yyy, 0, false, world->items.at(xxx + (yyy * world->width)).vprice, world->items.at(xxx + (yyy * world->width)).background, world->items.at(xxx + (yyy * world->width)).opened);
											}
											else UpdateVend(currentPeer, xxx, yyy, world->items.at(xxx + (yyy * world->width)).vid, false, world->items.at(xxx + (yyy * world->width)).vprice, world->items.at(xxx + (yyy * world->width)).background, world->items.at(xxx + (yyy * world->width)).opened);
										}
									}
								}
								else
								{
									world->items.at(xxx + (yyy * world->width)).vprice = 0;
									bool success = true;
									SaveItemMoreTimes(realid, countid, peer, success, pData->rawName + " collected from Vending Machine");
									world->items.at(xxx + (yyy * world->width)).vid = 0;
									world->items.at(xxx + (yyy * world->width)).vcount = 0;
									Player::OnTalkBubble(peer, pData->netID, "`wYou picked up " + to_string(countid) + " " + getItemDef(realid).name + "`w.", 0, true);
									Player::OnConsoleMessage(peer, "`oYou picked up " + to_string(countid) + " " + getItemDef(realid).name + "`o.");
									ENetPeer* currentPeer;
									for (currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
										if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
										if (isHere(peer, currentPeer)) {
											if (world->items.at(xxx + (yyy * world->width)).opened && world->items.at(xxx + (yyy * world->width)).vcount < world->items.at(xxx + (yyy * world->width)).vprice) {
												UpdateVend(currentPeer, xxx, yyy, 0, false, world->items.at(xxx + (yyy * world->width)).vprice, world->items.at(xxx + (yyy * world->width)).background, world->items.at(xxx + (yyy * world->width)).opened);
											}
											else UpdateVend(currentPeer, xxx, yyy, 0, false, 0, world->items.at(xxx + (yyy * world->width)).background, world->items.at(xxx + (yyy * world->width)).opened);
										}
									}
								}
							}
							catch (const std::out_of_range& e) {
								std::cout << e.what() << std::endl;
							}
						}
						if (btn == "withdraw")
						{
							if (world->owner != "" && !isWorldOwner(peer, world)) continue;
							int xxx = pData->lastPunchX;
							int yyy = pData->lastPunchY;
							if (xxx < 0 && yyy < 0) continue;
							try {
								int withdrawcpy = world->items.at(xxx + (yyy * world->width)).vdraw;
								if (withdrawcpy != 0) {
									if (withdrawcpy <= 200) {
										if (CheckItemMaxed(peer, 242, withdrawcpy))
										{
											Player::OnTalkBubble(peer, pData->netID, "`wI dont have free space to collect " + to_string(withdrawcpy) + " World Locks!", 0, true);
											continue;
										}
										world->items.at(xxx + (yyy * world->width)).vdraw = 0;
										int realid = world->items.at(xxx + (yyy * world->width)).vid;
										int priceid = world->items.at(xxx + (yyy * world->width)).vprice;
										bool success = true;
										SaveItemMoreTimes(242, withdrawcpy, peer, success, pData->rawName + " collected from Vending Machine");
										Player::OnTalkBubble(peer, pData->netID, "`wYou collected " + to_string(withdrawcpy) + " World Locks.", 0, true);
										if (world->items.at(xxx + (yyy * world->width)).vcount == 0) {
											priceid = 0;
											realid = 0;
										}
										ENetPeer* currentPeer;
										for (currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
											if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
											if (isHere(peer, currentPeer)) {
												if (world->items.at(xxx + (yyy * world->width)).opened && world->items.at(xxx + (yyy * world->width)).vcount < world->items.at(xxx + (yyy * world->width)).vprice) {
													UpdateVend(currentPeer, xxx, yyy, 0, false, priceid, world->items.at(xxx + (yyy * world->width)).background, world->items.at(xxx + (yyy * world->width)).opened);
												}
												else UpdateVend(currentPeer, xxx, yyy, realid, false, priceid, world->items.at(xxx + (yyy * world->width)).background, world->items.at(xxx + (yyy * world->width)).opened);
											}
										}
									}
									else if (withdrawcpy > 200)
									{
										if (CheckItemMaxed(peer, 242, 200))
										{
											Player::OnTalkBubble(peer, pData->netID, "`wI dont have free space to collect 200 World Locks!", 0, true);
											continue;
										}
										int grazinti = withdrawcpy - 200;
										world->items.at(xxx + (yyy * world->width)).vdraw = grazinti;
										int realid = world->items.at(xxx + (yyy * world->width)).vid;
										int priceid = world->items.at(xxx + (yyy * world->width)).vprice;
										bool success = true;
										SaveItemMoreTimes(242, 200, peer, success, pData->rawName + " collected from Vending Machine");
										Player::OnTalkBubble(peer, pData->netID, "`wYou collected 200 World Locks, leaving " + to_string(grazinti) + " in the machine!", 0, true);
										if (world->items.at(xxx + (yyy * world->width)).vcount == 0) {
											priceid = 0;
											realid = 0;
										}
										ENetPeer* currentPeer;
										for (currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
											if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
											if (isHere(peer, currentPeer)) {
												if (world->items.at(xxx + (yyy * world->width)).opened && world->items.at(xxx + (yyy * world->width)).vcount < world->items.at(xxx + (yyy * world->width)).vprice) {
													UpdateVend(currentPeer, xxx, yyy, 0, true, priceid, world->items.at(xxx + (yyy * world->width)).background, world->items.at(xxx + (yyy * world->width)).opened);
												}
												else UpdateVend(currentPeer, xxx, yyy, realid, true, priceid, world->items.at(xxx + (yyy * world->width)).background, world->items.at(xxx + (yyy * world->width)).opened);
											}
										}
									}
									else {
										Player::OnTalkBubble(peer, pData->netID, "`wHuh?", 0, true);
									}
								}
							}
							catch (const std::out_of_range& e) {
								std::cout << e.what() << std::endl;
							}
						}
						if (btn == "manipulatorstop") {
							if (world == nullptr || pData->currentWorld == "EXIT" || serverIsFrozen) continue;
							int x = pData->lastPunchX;
							int y = pData->lastPunchY;
							try {
								world->items.at(x + (y * world->width)).mid = 0;
								ENetPeer* currentPeer;
								for (currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
									if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
									if (isHere(peer, currentPeer)) {
										send_item_sucker(currentPeer, 6952, x, y, world->items.at(x + (y * world->width)).mid, 0, false, false, world->items.at(x + (y * world->width)).background);
									}
								}
							}
							catch (const std::out_of_range& e) {
								std::cout << e.what() << std::endl;
							}
						}
						if (btn == "organicstop") {
							if (world == nullptr || pData->currentWorld == "EXIT" || serverIsFrozen) continue;
							int x = pData->lastPunchX;
							int y = pData->lastPunchY;
							try {
								if (world->items.at(x + (y * world->width)).mc > 250) {
									if (CheckItemMaxed(peer, world->items.at(x + (y * world->width)).mid, 250)) {
										Player::OnTalkBubble(peer, pData->netID, "`wI dont have a free space to pickup `2250 `$" + getItemDef(world->items.at(x + (y * world->width)).mid).name + "`w!", 0, true);
										continue;
									}
									else {
										world->items.at(x + (y * world->width)).mc -= 250;
										bool success = true;
										SaveItemMoreTimes(world->items.at(x + (y * world->width)).mid, 250, peer, success, pData->rawName + " withdraw from Techno-Organic Engine");
										Player::OnTalkBubble(peer, pData->netID, "`oCollected `2250 `5" + getItemDef(world->items.at(x + (y * world->width)).mid).name + " `oFrom the Techno-Organic Engine!", 0, true);
										ENetPeer* currentPeer;
										for (currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
											if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
											if (isHere(peer, currentPeer)) {
												send_item_sucker(currentPeer, 6954, x, y, world->items.at(x + (y * world->width)).mid, 1, true, true, world->items.at(x + (y * world->width)).background);
											}
										}
									}
								}
								else
								{
									if (CheckItemMaxed(peer, world->items.at(x + (y * world->width)).mid, world->items.at(x + (y * world->width)).mc)) {
										Player::OnTalkBubble(peer, pData->netID, "`wI dont have a free space to pickup `2" + to_string(world->items.at(x + (y * world->width)).mc) + " `$" + getItemDef(world->items.at(x + (y * world->width)).mid).name + "`w!", 0, true);
										continue;
									}
									else {
										bool success = true;
										SaveItemMoreTimes(world->items.at(x + (y * world->width)).mid, world->items.at(x + (y * world->width)).mc, peer, success, pData->rawName + " withdraw from Techno-Organic Engine");
										Player::OnTalkBubble(peer, pData->netID, "`oCollected `2" + to_string(world->items.at(x + (y * world->width)).mc) + " `5" + getItemDef(world->items.at(x + (y * world->width)).mid).name + " `oFrom the Techno-Organic Engine!", 0, true);
										world->items.at(x + (y * world->width)).mid = 0;
										ENetPeer* currentPeer;
										for (currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
											if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
											if (isHere(peer, currentPeer)) {
												send_item_sucker(currentPeer, 6954, x, y, world->items.at(x + (y * world->width)).mid, -1, true, true, world->items.at(x + (y * world->width)).background);
											}
										}
										world->items.at(x + (y * world->width)).mc = 0;
									}
								}
							}
							catch (const std::out_of_range& e) {
								std::cout << e.what() << std::endl;
							}
						}
						if (btn == "addorganic") {
							if (world->owner != "" && !isWorldOwner(peer, world)) continue;
							int xxx = pData->lastPunchX;
							int yyy = pData->lastPunchY;
							if (xxx < 0 && yyy < 0) continue;
							try {
								int iditem = world->items.at(xxx + (yyy * world->width)).mid;
								int countitem = world->items.at(xxx + (yyy * world->width)).mc;
								auto mtitems = 0;
								for (auto i = 0; i < pData->inventory.items.size(); i++) {
									if (pData->inventory.items.at(i).itemID == iditem) {
										mtitems = pData->inventory.items.at(i).itemCount;
										break;
									}
								}
								auto iscontainseas = false;
								SearchInventoryItem(peer, iditem, mtitems, iscontainseas);
								if (!iscontainseas) {
									Player::OnTalkBubble(peer, pData->netID, "Huh?", 0, true);
									continue;
								}
								else {
									if (countitem + mtitems > 5000) {
										Player::OnTalkBubble(peer, pData->netID, "The machine will really explode if you will fit more of them into it!", 0, true);
										continue;
									}
									RemoveInventoryItem(iditem, mtitems, peer, true);
									countitem += mtitems;
									world->items.at(xxx + (yyy * world->width)).mc = countitem;
									Player::OnTalkBubble(peer, pData->netID, "`wAdded " + to_string(mtitems) + " to the machine.", 0, true);
									ENetPeer* currentPeer;
									for (currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
										if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
										if (isHere(peer, currentPeer)) {
											send_item_sucker(currentPeer, 6954, xxx, yyy, world->items.at(xxx + (yyy * world->width)).mid, 1, true, true, world->items.at(xxx + (yyy * world->width)).background);
										}
									}
								}
							}
							catch (const std::out_of_range& e) {
								std::cout << e.what() << std::endl;
							}
						}
						if (btn == "addvend") {
							if (world->owner != "" && !isWorldOwner(peer, world) && !mod(peer)) continue;
							int xxx = pData->lastPunchX;
							int yyy = pData->lastPunchY;
							if (xxx < 0 && yyy < 0) continue;
							try {
								int iditem = world->items.at(xxx + (yyy * world->width)).vid;
								int countitem = world->items.at(xxx + (yyy * world->width)).vcount;
								auto mtitems = 0;
								for (auto i = 0; i < pData->inventory.items.size(); i++) {
									if (pData->inventory.items.at(i).itemID == iditem) {
										mtitems = pData->inventory.items.at(i).itemCount;
										break;
									}
								}
								auto iscontainseas = false;
								SearchInventoryItem(peer, iditem, mtitems, iscontainseas);
								if (!iscontainseas) {
									Player::OnTalkBubble(peer, pData->netID, "Huh?", 0, true);
									continue;
								}
								else {
									RemoveInventoryItem(iditem, mtitems, peer, true);
									updateplayerset(peer, iditem);
									countitem += mtitems;
									world->items.at(xxx + (yyy * world->width)).vcount = countitem;
									Player::OnTalkBubble(peer, pData->netID, "`wAdded " + to_string(mtitems) + " to the machine.", 0, true);
								}
							}
							catch (const std::out_of_range& e) {
								std::cout << e.what() << std::endl;
							}
						}
						if (btn == "addletter")
						{
							if (world == nullptr || pData->currentWorld == "EXIT" || serverIsFrozen) continue;
							try {
								std::stringstream bss(cch);
								std::string bto;

								string text = "";
								string coord = "";

								while (std::getline(bss, bto, '\n'))
								{
									vector<string> infoDat = explode("|", bto);
									if (infoDat.at(0).substr(0, 15) == "addletterinput_")
									{
										text = infoDat.at(1);
										string coord = infoDat.at(0).substr(infoDat.at(0).find("_") + 1);
										auto isdbox = std::experimental::filesystem::exists("save/mailbox/_" + pData->currentWorld + "/X" + coord + ".json");
										if (!isdbox)
										{
											Player::OnTalkBubble(peer, pData->netID, "`4An error occured. Break the mailbox.", 0, true);
											break;
										}

										if (text.size() < 3)
										{
											Player::OnTalkBubble(peer, pData->netID, "`wThats not interesting enough to mail.", 0, true);
											break;
										}

										ifstream ifff("save/mailbox/_" + pData->currentWorld + "/X" + coord + ".json");
										json j;
										ifff >> j;
										ifff.close();

										if (j["inmail"] > 19)
										{
											Player::OnTalkBubble(peer, pData->netID, "`oThis mailbox already has `w20 `oletters in it. Try again later.", 0, true);
										}
										for (int i = 0; i < 20; i++)
										{
											if (j["mails"].at(i)["growid"] == "")
											{
												j["mails"].at(i)["growid"] = pData->displayName;
												j["mails"].at(i)["text"] = text;
												Player::OnTalkBubble(peer, pData->netID, "`2You place your letter in the mailbox.", 0, true);
												int x = j["x"];
												int y = j["y"];
												j["inmail"] = j["inmail"].get<double>() + 1;
												ofstream of("save/mailbox/_" + pData->currentWorld + "/X" + coord + ".json");
												of << j << std::endl;
												of.close();
												break;
											}
										}
									}
								}
							}
							catch (const std::out_of_range& e)
							{
								std::cout << e.what() << std::endl;
							}
						}
						if (btn == "addblueletter")
						{
							if (world == nullptr || pData->currentWorld == "EXIT" || serverIsFrozen) continue;
							try {
								std::stringstream bss(cch);
								std::string bto;

								string text = "";

								while (std::getline(bss, bto, '\n'))
								{
									vector<string> infoDat = explode("|", bto);
									if (infoDat.at(0).substr(0, 19) == "addblueletterinput_")
									{
										text = infoDat.at(1);
										string coord = infoDat.at(0).substr(infoDat.at(0).find("_") + 1);
										auto isdbox = std::experimental::filesystem::exists("save/bluemailbox/_" + pData->currentWorld + "/X" + coord + ".json");
										if (!isdbox)
										{
											Player::OnTalkBubble(peer, pData->netID, "`4An error occured. Break the blue mailbox.", 0, true);
											break;
										}

										if (text.size() < 3)
										{
											Player::OnTalkBubble(peer, pData->netID, "`wThats not interesting enough to mail.", 0, true);
											break;
										}

										ifstream ifff("save/bluemailbox/_" + pData->currentWorld + "/X" + coord + ".json");
										json j;
										ifff >> j;
										ifff.close();

										if (j["inmail"] > 19)
										{
											Player::OnTalkBubble(peer, pData->netID, "`oThis mailbox already has `w20 `oletters in it. Try again later.", 0, true);
										}
										for (int i = 0; i < 20; i++)
										{
											if (j["mails"].at(i)["growid"] == "")
											{
												j["mails"].at(i)["growid"] = pData->displayName;
												j["mails"].at(i)["text"] = text;
												Player::OnTalkBubble(peer, pData->netID, "`2You place your letter in the mailbox.", 0, true);
												int x = j["x"];
												int y = j["y"];
												if (j["inmail"] < 1)
												{
													ENetPeer* currentPeer;
													for (currentPeer = server->peers;
														currentPeer < &server->peers[server->peerCount];
														++currentPeer)
													{
														if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
													}
												}
												j["inmail"] = j["inmail"].get<double>() + 1;
												ofstream of("save/bluemailbox/_" + pData->currentWorld + "/X" + coord + ".json");
												of << j << std::endl;
												of.close();
												break;
											}
										}
									}
								}
							}
							catch (const std::out_of_range& e) {
								std::cout << e.what() << std::endl;
							}
						}
						if (btn == "boxlvl2depositconfirm")
						{
							if (world == nullptr || pData->currentWorld == "EXIT" || serverIsFrozen) continue;
							try {
								std::stringstream bss(cch);
								std::string bto;

								string count = "";
								int countint = 0;
								string coord = "";
								string id = "";
								int idint = 0;

								while (std::getline(bss, bto, '\n'))
								{
									vector<string> infoDat = explode("|", bto);
									if (infoDat.at(0).substr(0, 20) == "boxlvl2depositcount_")
									{
										count = infoDat.at(1);

										string coord_and_id = infoDat.at(0).erase(0, 20);
										string::size_type pos = coord_and_id.find('_');
										if (pos != std::string::npos)
										{
											coord = coord_and_id.substr(0, pos);
											coord_and_id.erase(0, pos + 1);
											id = coord_and_id;
										}
										else
										{
											break;
										}
										auto isdbox = std::experimental::filesystem::exists("save/storageboxlvl2/_" + pData->currentWorld + "/X" + coord + ".json");
										if (!isdbox)
										{
											Player::OnTalkBubble(peer, pData->netID, "`4An error occured while getting box info.", 0, true);
											break;
										}

										bool contains_non_int3 = !std::regex_match(id, std::regex("^[0-9]+$"));
										if (contains_non_int3 == true)
										{
											break;
										}

										bool contains_non_int4 = !std::regex_match(count, std::regex("^[0-9]+$"));
										if (contains_non_int4 == true)
										{
											Player::OnTalkBubble(peer, pData->netID, "Invalid input in amount!", 0, false);
											break;
										}
										countint = atoi(count.c_str());

										idint = atoi(id.c_str());
										if (idint < 1 || idint > maxItems)
										{
											break;
										}

										if (countint < 1 || countint > 250)
										{
											Player::OnTalkBubble(peer, pData->netID, "Invalid input in amount!", 0, false);
											break;
										}

										ifstream ifff("save/storageboxlvl2/_" + pData->currentWorld + "/X" + coord + ".json");
										json j;
										ifff >> j;
										ifff.close();

										if (j["instorage"] > 19)
										{
											Player::OnTalkBubble(peer, pData->netID, "`4Storage Box Xtreme - Level 2 is full!", 0, true);
											break;
										}
										bool isContains = false;
										SearchInventoryItem(peer, idint, countint, isContains);
										if (isContains == false)
										{
											Player::OnTextOverlay(peer, "`4You don't have enough " + itemDefs[idint].name + " in your inventory.");
											break;
										}
										j["instorage"] = j["instorage"].get<double>() + 1;

										for (int i = 0; i < 20; i++)
										{
											if (j["storage"].at(i)["itemid"] == 0)
											{
												j["storage"].at(i)["itemid"] = idint;
												j["storage"].at(i)["placedby"] = pData->rawName;
												j["storage"].at(i)["itemcount"] = countint;

												Player::OnTalkBubble(peer, pData->netID, "`wStored " + to_string(countint) + " " + itemDefs[idint].name + " `win the Storage Box Xtreme - Level 2", 0, true);
												break;
											}
										}

										ofstream of("save/storageboxlvl2/_" + pData->currentWorld + "/X" + coord + ".json");
										of << j << std::endl;
										of.close();
										RemoveInventoryItem(idint, countint, peer, true);
										updateplayerset(peer, idint);
									}
								}
							}
							catch (const std::out_of_range& e) {
								std::cout << e.what() << std::endl;
							}
						}
						if (btn == "boxlvl3depositconfirm")
						{
							if (world == nullptr || pData->currentWorld == "EXIT" || serverIsFrozen) continue;
							try {
								std::stringstream bss(cch);
								std::string bto;

								string count = "";
								int countint = 0;
								string coord = "";
								string id = "";
								int idint = 0;

								while (std::getline(bss, bto, '\n'))
								{
									vector<string> infoDat = explode("|", bto);
									if (infoDat.at(0).substr(0, 20) == "boxlvl3depositcount_")
									{
										count = infoDat.at(1);

										string coord_and_id = infoDat.at(0).erase(0, 20);
										string::size_type pos = coord_and_id.find('_');
										if (pos != std::string::npos)
										{
											coord = coord_and_id.substr(0, pos);
											coord_and_id.erase(0, pos + 1);
											id = coord_and_id;
										}
										else
										{
											break;
										}
										auto isdbox = std::experimental::filesystem::exists("save/storageboxlvl3/_" + pData->currentWorld + "/X" + coord + ".json");
										if (!isdbox)
										{
											Player::OnTalkBubble(peer, pData->netID, "`4An error occured while getting box info.", 0, true);
											break;
										}

										bool contains_non_int3 = !std::regex_match(id, std::regex("^[0-9]+$"));
										if (contains_non_int3 == true)
										{
											break;
										}

										bool contains_non_int4 = !std::regex_match(count, std::regex("^[0-9]+$"));
										if (contains_non_int4 == true)
										{
											Player::OnTalkBubble(peer, pData->netID, "Invalid input in amount!", 0, false);
											break;
										}
										countint = atoi(count.c_str());

										idint = atoi(id.c_str());
										if (idint < 1 || idint > maxItems)
										{
											break;
										}

										if (countint < 1 || countint > 250)
										{
											Player::OnTalkBubble(peer, pData->netID, "Invalid input in amount!", 0, false);
											break;
										}

										ifstream ifff("save/storageboxlvl3/_" + pData->currentWorld + "/X" + coord + ".json");
										json j;
										ifff >> j;
										ifff.close();

										if (j["instorage"] > 19)
										{
											Player::OnTalkBubble(peer, pData->netID, "`4Storage Box Xtreme - Level 3 is full!", 0, true);
											break;
										}
										bool isContains = false;
										SearchInventoryItem(peer, idint, countint, isContains);
										if (isContains == false)
										{
											Player::OnTextOverlay(peer, "`4You don't have enough " + itemDefs[idint].name + " in your inventory.");
											break;
										}
										j["instorage"] = j["instorage"].get<double>() + 1;

										for (int i = 0; i < 20; i++)
										{
											if (j["storage"].at(i)["itemid"] == 0)
											{
												j["storage"].at(i)["itemid"] = idint;
												j["storage"].at(i)["placedby"] = pData->rawName;
												j["storage"].at(i)["itemcount"] = countint;

												Player::OnTalkBubble(peer, pData->netID, "`wStored " + to_string(countint) + " " + itemDefs[idint].name + " `win the Storage Box Xtreme - Level 3", 0, true);
												break;
											}
										}
										ofstream of("save/storageboxlvl3/_" + pData->currentWorld + "/X" + coord + ".json");
										of << j << std::endl;
										of.close();
										RemoveInventoryItem(idint, countint, peer, true);
										updateplayerset(peer, idint);
									}
								}
							}
							catch (const std::out_of_range& e) {
								std::cout << e.what() << std::endl;
							}
						}
						if (btn == "boxlvl1depositconfirm")
						{
							if (world == nullptr || pData->currentWorld == "EXIT" || serverIsFrozen) continue;
							try {
								std::stringstream bss(cch);
								std::string bto;

								string count = "";
								int countint = 0;
								string coord = "";
								string id = "";
								int idint = 0;

								while (std::getline(bss, bto, '\n'))
								{
									vector<string> infoDat = explode("|", bto);
									if (infoDat.at(0).substr(0, 20) == "boxlvl1depositcount_")
									{
										count = infoDat.at(1);

										string coord_and_id = infoDat.at(0).erase(0, 20);
										string::size_type pos = coord_and_id.find('_');
										if (pos != std::string::npos)
										{
											coord = coord_and_id.substr(0, pos);
											coord_and_id.erase(0, pos + 1);
											id = coord_and_id;
										}
										else
										{
											break;
										}
										auto isdbox = std::experimental::filesystem::exists("save/storageboxlvl1/_" + pData->currentWorld + "/X" + coord + ".json");
										if (!isdbox)
										{
											Player::OnTalkBubble(peer, pData->netID, "`4An error occured while getting box info.", 0, true);
											break;
										}

										bool contains_non_int3 = !std::regex_match(id, std::regex("^[0-9]+$"));
										if (contains_non_int3 == true)
										{
											break;
										}

										bool contains_non_int4 = !std::regex_match(count, std::regex("^[0-9]+$"));
										if (contains_non_int4 == true)
										{
											Player::OnTalkBubble(peer, pData->netID, "Invalid input in amount!", 0, false);
											break;
										}
										countint = atoi(count.c_str());

										idint = atoi(id.c_str());
										if (idint < 1 || idint > maxItems)
										{
											break;
										}

										if (countint < 1 || countint > 250)
										{
											Player::OnTalkBubble(peer, pData->netID, "Invalid input in amount!", 0, false);
											break;
										}

										ifstream ifff("save/storageboxlvl1/_" + pData->currentWorld + "/X" + coord + ".json");
										json j;
										ifff >> j;
										ifff.close();

										if (j["instorage"] > 19)
										{
											Player::OnTalkBubble(peer, pData->netID, "`4Storage Box Xtreme - Level 1 is full!", 0, true);
											break;
										}
										bool isContains = false;
										SearchInventoryItem(peer, idint, countint, isContains);
										if (isContains == false)
										{
											Player::OnTextOverlay(peer, "`4You don't have enough " + itemDefs[idint].name + " in your inventory.");
											break;
										}
										j["instorage"] = j["instorage"].get<double>() + 1;

										for (int i = 0; i < 20; i++)
										{
											if (j["storage"].at(i)["itemid"] == 0)
											{
												j["storage"].at(i)["itemid"] = idint;
												j["storage"].at(i)["placedby"] = pData->rawName;
												j["storage"].at(i)["itemcount"] = countint;

												Player::OnTalkBubble(peer, pData->netID, "`wStored " + to_string(countint) + " " + itemDefs[idint].name + " `win the Storage Box Xtreme - Level 1", 0, true);
												break;
											}
										}

										ofstream of("save/storageboxlvl1/_" + pData->currentWorld + "/X" + coord + ".json");
										of << j << std::endl;
										of.close();
										RemoveInventoryItem(idint, countint, peer, true);
										updateplayerset(peer, idint);
									}
								}
							}
							catch (const std::out_of_range& e) {
								std::cout << e.what() << std::endl;
							}
						}
						if (btn == "boxlvl2_withdrawConfirm")
						{
							if (world == nullptr || pData->currentWorld == "EXIT" || serverIsFrozen) continue;
							try {
								std::stringstream bss(cch);
								std::string bto;

								string howMany = "";
								string apos = "";
								string coord = "";
								int aposInt = 0;
								int howManyInt = 0;

								while (std::getline(bss, bto, '\n'))
								{
									vector<string> infoDat = explode("|", bto);
									if (infoDat.at(0).substr(0, 16) == "boxlvl2withdraw_")
									{
										howMany = infoDat.at(1);
										if (howMany.size() > 5) break;

										string coord_and_apos = infoDat.at(0).erase(0, 16);
										string::size_type pos = coord_and_apos.find('_');
										if (pos != std::string::npos)
										{
											coord = coord_and_apos.substr(0, pos);
											coord_and_apos.erase(0, pos + 1);
											apos = coord_and_apos;
										}
										else
										{
											break;
										}
										auto isdbox = std::experimental::filesystem::exists("save/storageboxlvl2/_" + pData->currentWorld + "/X" + coord + ".json");
										if (!isdbox)
										{
											Player::OnTalkBubble(peer, pData->netID, "`4An error occured while getting safe vault info.", 0, true);
											break;
										}

										bool contains_non_int3 = !std::regex_match(apos, std::regex("^[0-9]+$"));
										if (contains_non_int3 == true)
										{
											break;
										}

										bool contains_non_int4 = !std::regex_match(howMany, std::regex("^[0-9]+$"));
										if (contains_non_int4 == true)
										{
											Player::OnTalkBubble(peer, pData->netID, "Invalid input in amount!", 0, false);
											break;
										}
										howManyInt = atoi(howMany.c_str());

										aposInt = atoi(apos.c_str());

										if (howManyInt < 1 || howManyInt > 250)
										{
											Player::OnTalkBubble(peer, pData->netID, "Invalid input in amount!", 0, false);
											break;
										}

										ifstream ifff("save/storageboxlvl2/_" + pData->currentWorld + "/X" + coord + ".json");
										json j;
										ifff >> j;
										ifff.close();

										int itemid = 0;
										bool success = true;

										for (int i = 0; i < 20; i++)
										{
											if (j["storage"].at(i)["aposition"] == aposInt)
											{
												itemid = j["storage"].at(i)["itemid"];

												if (itemid == 0) break;

												if (howManyInt > j["storage"].at(i)["itemcount"])
												{
													Player::OnTextOverlay(peer, "`4Your Storage Box Xtreme - Level 2 does not contains such many.");
													break;
												}

												if (CheckItemExists(peer, j["storage"].at(i)["itemid"]))
												{
													int currentlyHave = GetQuantityOfItem(peer, j["storage"].at(i)["itemid"]);
													if (howManyInt + currentlyHave < 251)
													{
														SaveItemMoreTimes(j["storage"].at(i)["itemid"], howManyInt, peer, success, pData->rawName + " from Storage Box Xtreme - Level 2");
													}
													else
													{
														Player::OnTextOverlay(peer, "`4You don't have enought space.");
														break;
													}
												}
												else
												{
													if (pData->inventory.items.size() == pData->currentInventorySize)
													{
														Player::OnTextOverlay(peer, "`4Your inventory is full! You can purchase an inventory upgrade in the shop.");
														break;
													}

													SaveItemMoreTimes(j["storage"].at(i)["itemid"], howManyInt, peer, success, pData->rawName + " from Storage Box Xtreme - Level 2");
												}
												Player::OnTalkBubble(peer, pData->netID, "`wRemoved " + to_string(howManyInt) + " " + itemDefs[itemid].name + " from the Storage Box Xtreme - Level 2", 0, true);
												if (j["storage"].at(i)["itemcount"] == howManyInt)
												{
													j["storage"].at(i)["itemid"] = 0;
													j["storage"].at(i)["placedby"] = "There was " + to_string(howManyInt) + " of " + to_string(itemid) + " item, and " + pData->rawName + " withdrawed them.";
													j["storage"].at(i)["itemcount"] = 0;
													j["instorage"] = j["instorage"].get<double>() - 1;
												}
												else
												{
													j["storage"].at(i)["itemcount"] = j["storage"].at(i)["itemcount"].get<double>() - howManyInt;
												}

												ofstream of("save/storageboxlvl2/_" + pData->currentWorld + "/X" + coord + ".json");
												of << j << std::endl;
												of.close();
												break;
											}
										}
									}
								}
							}
							catch (const std::out_of_range& e) {
								std::cout << e.what() << std::endl;
							}
						}
						if (btn == "boxlvl3_withdrawConfirm")
						{
							if (world == nullptr || pData->currentWorld == "EXIT" || serverIsFrozen) continue;
							try {
								std::stringstream bss(cch);
								std::string bto;

								string howMany = "";
								string apos = "";
								string coord = "";
								int aposInt = 0;
								int howManyInt = 0;

								while (std::getline(bss, bto, '\n'))
								{
									vector<string> infoDat = explode("|", bto);
									if (infoDat.at(0).substr(0, 16) == "boxlvl3withdraw_")
									{
										howMany = infoDat.at(1);
										if (howMany.size() > 5) break;

										string coord_and_apos = infoDat.at(0).erase(0, 16);
										string::size_type pos = coord_and_apos.find('_');
										if (pos != std::string::npos)
										{
											coord = coord_and_apos.substr(0, pos);
											coord_and_apos.erase(0, pos + 1);
											apos = coord_and_apos;
										}
										else
										{
											break;
										}
										auto isdbox = std::experimental::filesystem::exists("save/storageboxlvl3/_" + pData->currentWorld + "/X" + coord + ".json");
										if (!isdbox)
										{
											Player::OnTalkBubble(peer, pData->netID, "`4An error occured while getting safe vault info.", 0, true);
											break;
										}

										bool contains_non_int3 = !std::regex_match(apos, std::regex("^[0-9]+$"));
										if (contains_non_int3 == true)
										{
											break;
										}

										bool contains_non_int4 = !std::regex_match(howMany, std::regex("^[0-9]+$"));
										if (contains_non_int4 == true)
										{
											Player::OnTalkBubble(peer, pData->netID, "Invalid input in amount!", 0, false);
											break;
										}
										howManyInt = atoi(howMany.c_str());

										aposInt = atoi(apos.c_str());

										if (howManyInt < 1 || howManyInt > 250)
										{
											Player::OnTalkBubble(peer, pData->netID, "Invalid input in amount!", 0, false);
											break;
										}

										ifstream ifff("save/storageboxlvl3/_" + pData->currentWorld + "/X" + coord + ".json");
										json j;
										ifff >> j;
										ifff.close();

										int itemid = 0;
										bool success = true;

										for (int i = 0; i < 20; i++)
										{
											if (j["storage"].at(i)["aposition"] == aposInt)
											{
												itemid = j["storage"].at(i)["itemid"];

												if (itemid == 0) break;

												if (howManyInt > j["storage"].at(i)["itemcount"])
												{
													Player::OnTextOverlay(peer, "`4Your Storage Box Xtreme - Level 3 does not contains such many.");
													break;
												}

												if (CheckItemExists(peer, j["storage"].at(i)["itemid"]))
												{
													int currentlyHave = GetQuantityOfItem(peer, j["storage"].at(i)["itemid"]);
													if (howManyInt + currentlyHave < 251)
													{
														SaveItemMoreTimes(j["storage"].at(i)["itemid"], howManyInt, peer, success, pData->rawName + " from Storage Box Xtreme - Level 3");
													}
													else
													{
														Player::OnTextOverlay(peer, "`4You don't have enought space.");
														break;
													}
												}
												else
												{
													if (pData->inventory.items.size() == pData->currentInventorySize)
													{
														Player::OnTextOverlay(peer, "`4Your inventory is full! You can purchase an inventory upgrade in the shop.");
														break;
													}

													SaveItemMoreTimes(j["storage"].at(i)["itemid"], howManyInt, peer, success, pData->rawName + " from Storage Box Xtreme - Level 3");
												}
												Player::OnTalkBubble(peer, pData->netID, "`wRemoved " + to_string(howManyInt) + " " + itemDefs[itemid].name + " from the Storage Box Xtreme - Level 3", 0, true);
												if (j["storage"].at(i)["itemcount"] == howManyInt)
												{
													j["storage"].at(i)["itemid"] = 0;
													j["storage"].at(i)["placedby"] = "There was " + to_string(howManyInt) + " of " + to_string(itemid) + " item, and " + pData->rawName + " withdrawed them.";
													j["storage"].at(i)["itemcount"] = 0;
													j["instorage"] = j["instorage"].get<double>() - 1;
												}
												else
												{
													j["storage"].at(i)["itemcount"] = j["storage"].at(i)["itemcount"].get<double>() - howManyInt;
												}

												ofstream of("save/storageboxlvl3/_" + pData->currentWorld + "/X" + coord + ".json");
												of << j << std::endl;
												of.close();
												break;
											}
										}
									}
								}
							}
							catch (const std::out_of_range& e) {
								std::cout << e.what() << std::endl;
							}
						}
						if (btn == "boxlvl1_withdrawConfirm")
						{
							if (world == nullptr || pData->currentWorld == "EXIT" || serverIsFrozen) continue;
							try {
								std::stringstream bss(cch);
								std::string bto;

								string howMany = "";
								string apos = "";
								string coord = "";
								int aposInt = 0;
								int howManyInt = 0;

								while (std::getline(bss, bto, '\n'))
								{
									vector<string> infoDat = explode("|", bto);
									if (infoDat.at(0).substr(0, 16) == "boxlvl1withdraw_")
									{
										howMany = infoDat.at(1);
										if (howMany.size() > 5) break;

										string coord_and_apos = infoDat.at(0).erase(0, 16);
										string::size_type pos = coord_and_apos.find('_');
										if (pos != std::string::npos)
										{
											coord = coord_and_apos.substr(0, pos);
											coord_and_apos.erase(0, pos + 1);
											apos = coord_and_apos;
										}
										else
										{
											break;
										}
										auto isdbox = std::experimental::filesystem::exists("save/storageboxlvl1/_" + pData->currentWorld + "/X" + coord + ".json");
										if (!isdbox)
										{
											Player::OnTalkBubble(peer, pData->netID, "`4An error occured while getting safe vault info.", 0, true);
											break;
										}

										bool contains_non_int3 = !std::regex_match(apos, std::regex("^[0-9]+$"));
										if (contains_non_int3 == true)
										{
											break;
										}

										bool contains_non_int4 = !std::regex_match(howMany, std::regex("^[0-9]+$"));
										if (contains_non_int4 == true)
										{
											Player::OnTalkBubble(peer, pData->netID, "Invalid input in amount!", 0, false);
											break;
										}
										howManyInt = atoi(howMany.c_str());

										aposInt = atoi(apos.c_str());

										if (howManyInt < 1 || howManyInt > 250)
										{
											Player::OnTalkBubble(peer, pData->netID, "Invalid input in amount!", 0, false);
											break;
										}

										ifstream ifff("save/storageboxlvl1/_" + pData->currentWorld + "/X" + coord + ".json");
										json j;
										ifff >> j;
										ifff.close();

										int itemid = 0;
										bool success = true;

										for (int i = 0; i < 20; i++)
										{
											if (j["storage"].at(i)["aposition"] == aposInt)
											{
												itemid = j["storage"].at(i)["itemid"];

												if (itemid == 0) break;

												if (howManyInt > j["storage"].at(i)["itemcount"])
												{
													Player::OnTextOverlay(peer, "`4Your Storage Box Xtreme - Level 1 does not contains such many.");
													break;
												}

												if (CheckItemExists(peer, j["storage"].at(i)["itemid"]))
												{
													int currentlyHave = GetQuantityOfItem(peer, j["storage"].at(i)["itemid"]);
													if (howManyInt + currentlyHave < 251)
													{
														SaveItemMoreTimes(j["storage"].at(i)["itemid"], howManyInt, peer, success, pData->rawName + " from Storage Box Xtreme - Level 1");
													}
													else
													{
														Player::OnTextOverlay(peer, "`4You don't have enought space.");
														break;
													}
												}
												else
												{
													if (pData->inventory.items.size() == pData->currentInventorySize)
													{
														Player::OnTextOverlay(peer, "`4Your inventory is full! You can purchase an inventory upgrade in the shop.");
														break;
													}
													SaveItemMoreTimes(j["storage"].at(i)["itemid"], howManyInt, peer, success, pData->rawName + " from Storage Box Xtreme - Level 1");
												}
												Player::OnTalkBubble(peer, pData->netID, "`wRemoved " + to_string(howManyInt) + " " + itemDefs[itemid].name + " from the Storage Box Xtreme - Level 1", 0, true);
												if (j["storage"].at(i)["itemcount"] == howManyInt)
												{
													j["storage"].at(i)["itemid"] = 0;
													j["storage"].at(i)["placedby"] = "There was " + to_string(howManyInt) + " of " + to_string(itemid) + " item, and " + pData->rawName + " withdrawed them.";
													j["storage"].at(i)["itemcount"] = 0;
													j["instorage"] = j["instorage"].get<double>() - 1;
												}
												else
												{
													j["storage"].at(i)["itemcount"] = j["storage"].at(i)["itemcount"].get<double>() - howManyInt;
												}

												ofstream of("save/storageboxlvl1/_" + pData->currentWorld + "/X" + coord + ".json");
												of << j << std::endl;
												of.close();
												break;
											}
										}
									}
								}
							}
							catch (const std::out_of_range& e) {
								std::cout << e.what() << std::endl;
							}
						}
						if (btn.substr(0, 21) == "safeBoxDepositedItem_")
						{
							if (world == nullptr || pData->currentWorld == "EXIT" || serverIsFrozen) continue;
							if (pData->rawName == PlayerDB::getProperName(world->owner) || world->owner == "" || mod(peer))
							{
								try {
									string apos_and_coord = btn.erase(0, 21);
									string::size_type pos = apos_and_coord.find('_');
									string apos = "";
									string coord = "";

									if (pos != std::string::npos)
									{
										apos = apos_and_coord.substr(0, pos);
										apos_and_coord.erase(0, pos + 1);
										coord = apos_and_coord;
									}
									else
									{
										break;
									}
									auto isdbox = std::experimental::filesystem::exists("save/safevault/_" + pData->currentWorld + "/X" + coord + ".json");
									if (!isdbox)
									{
										Player::OnTalkBubble(peer, pData->netID, "`4An error occured while getting safe vault info.", 0, true);
										break;
									}

									bool contains_non_int3 = !std::regex_match(apos, std::regex("^[0-9]+$"));
									if (contains_non_int3 == true)
									{
										break;
									}

									ifstream ifff("save/safevault/_" + pData->currentWorld + "/X" + coord + ".json");
									json j;
									ifff >> j;
									ifff.close();

									int itemid = 0;
									int count = 0;
									for (int i = 0; i < 20; i++)
									{
										if (j["safe"].at(i)["aposition"] == atoi(apos.c_str()))
										{
											itemid = j["safe"].at(i)["itemid"];
											count = j["safe"].at(i)["itemcount"];
											GTDialog mySafe;
											mySafe.addLabelWithIcon("`wSafe Vault", 8878, LABEL_BIG);
											mySafe.addSmallText("`oYou have `w" + to_string(count) + " " + itemDefs[itemid].name + " `ostored.");
											mySafe.addSmallText("`oWithdraw how many?");
											mySafe.addInputBox("safewithdraw_" + coord + "_" + apos, "", to_string(count), 5);
											mySafe.addSpacer(SPACER_SMALL);
											mySafe.addQuickExit();
											mySafe.endDialog("safe_withdrawConfirm", "Remove items", "Exit");
											Player::OnDialogRequest(peer, mySafe.finishDialog());
											break;
										}
									}
								}
								catch (const std::out_of_range& e) {
									std::cout << e.what() << std::endl;
								}
							}
						}
						if (btn.substr(0, 29) == "saferecoverPasswordInConfirm_")
						{
							if (pData->rawName == PlayerDB::getProperName(world->owner) || world->owner == "" || mod(peer))
							{
								if (world == nullptr || pData->currentWorld == "EXIT" || serverIsFrozen) continue;
								string coord = btn.substr(btn.find("_") + 1);
								auto isdbox = std::experimental::filesystem::exists("save/safevault/_" + pData->currentWorld + "/X" + coord + ".json");
								if (!isdbox)
								{
									Player::OnTalkBubble(peer, pData->netID, "`4An error occured. Break the safe.", 0, true);
									break;
								}
								GTDialog recover;
								recover.addLabelWithIcon("`wSafe Vault", 8878, LABEL_BIG);
								recover.addSmallText("Please enter recovery answer.");
								recover.addInputBox("saferecoveryanswer_" + coord, "", "", 12);
								recover.addButton("saferecoveryanswerconfirm", "Enter Recovery Answer");
								recover.addSpacer(SPACER_SMALL);
								recover.addQuickExit();
								recover.endDialog("Close", "", "Exit");
								Player::OnDialogRequest(peer, recover.finishDialog());
							}
						}
						if (btn == "saferecoveryanswerconfirm")
						{
							if (world == nullptr || pData->currentWorld == "EXIT" || serverIsFrozen) continue;
							try {
								std::stringstream bss(cch);
								std::string bto;

								string recoveryanswer = "";
								string coord = "";

								while (std::getline(bss, bto, '\n'))
								{
									vector<string> infoDat = explode("|", bto);
									if (infoDat.at(0).substr(0, 19) == "saferecoveryanswer_")
									{
										if (pData->rawName == PlayerDB::getProperName(world->owner) || world->owner == "" || mod(peer))
										{
											recoveryanswer = infoDat.at(1);
											coord = infoDat.at(0).substr(19, infoDat.at(0).length() - 1).c_str();
											auto isdbox = std::experimental::filesystem::exists("save/safevault/_" + pData->currentWorld + "/X" + coord + ".json");
											if (!isdbox)
											{
												Player::OnTalkBubble(peer, pData->netID, "`4An error occured. Break the safe.", 0, true);
												break;
											}

											ifstream ifff("save/safevault/_" + pData->currentWorld + "/X" + coord + ".json");
											json j;
											ifff >> j;
											ifff.close();

											if (recoveryanswer != j["recovery"])
											{
												Player::OnTalkBubble(peer, pData->netID, "`4Recovery answer did not match!", 0, true);
												break;
											}

											j["password"] = "";
											j["recovery"] = "";
											ofstream of("save/safevault/_" + pData->currentWorld + "/X" + coord + ".json");
											of << j << std::endl;
											of.close();

											Player::OnTalkBubble(peer, pData->netID, "Your safe vault was `2Updated!", 0, true);
										}
									}
								}
							}
							catch (const std::out_of_range& e) {
								std::cout << e.what() << std::endl;
							}
						}
						if (btn == "safe_updatepassword")
						{
							if (world == nullptr || pData->currentWorld == "EXIT" || serverIsFrozen) continue;
							try {
								std::stringstream bss(cch);
								std::string bto;

								string inputoldpass = "";
								string coord = "";
								string newpass = "";
								string recans = "";

								while (std::getline(bss, bto, '\n'))
								{
									vector<string> infoDat = explode("|", bto);
									if (infoDat.at(0).substr(0, 16) == "safeoldpassword_")
									{
										if (pData->rawName == PlayerDB::getProperName(world->owner) || world->owner == "" || mod(peer))
										{
											inputoldpass = infoDat.at(1);
											coord = infoDat.at(0).substr(16, infoDat.at(0).length() - 1).c_str();
											auto isdbox = std::experimental::filesystem::exists("save/safevault/_" + pData->currentWorld + "/X" + coord + ".json");
											if (!isdbox)
											{
												Player::OnTalkBubble(peer, pData->netID, "`4An error occured. Break the safe.", 0, true);
												break;
											}

											ifstream ifff("save/safevault/_" + pData->currentWorld + "/X" + coord + ".json");
											json j;
											ifff >> j;
											ifff.close();

											if (j["password"] != inputoldpass)
											{
												Player::OnTalkBubble(peer, pData->netID, "`4Old password did not match", 0, true);
												break;
											}
										}
									}
									if (infoDat.at(0) == "safe_newpassword")
									{
										if (pData->rawName == PlayerDB::getProperName(world->owner) || world->owner == "" || mod(peer))
										{
											newpass = infoDat.at(1);
										}
									}
									if (infoDat.at(0) == "storage_recoveryanswer")
									{
										if (pData->rawName == PlayerDB::getProperName(world->owner) || world->owner == "" || mod(peer))
										{
											recans = infoDat.at(1);
											ifstream ifff("save/safevault/_" + pData->currentWorld + "/X" + coord + ".json");
											json j;
											ifff >> j;
											ifff.close();

											j["password"] = newpass;
											j["recovery"] = recans;
											ofstream of("save/safevault/_" + pData->currentWorld + "/X" + coord + ".json");
											of << j << std::endl;
											of.close();

											Player::OnTalkBubble(peer, pData->netID, "Your safe vault was `2Updated!", 0, true);
										}
									}
								}
							}
							catch (const std::out_of_range& e) {
								std::cout << e.what() << std::endl;
							}
						}
						if (btn == "safe_removepassword")
						{
							if (world == nullptr || pData->currentWorld == "EXIT" || serverIsFrozen) continue;
							try {
								std::stringstream bss(cch);
								std::string bto;

								string inputoldpass = "";
								string coord = "";

								while (std::getline(bss, bto, '\n'))
								{
									vector<string> infoDat = explode("|", bto);
									if (infoDat.at(0).substr(0, 16) == "safeoldpassword_")
									{
										if (pData->rawName == PlayerDB::getProperName(world->owner) || world->owner == "" || mod(peer))
										{
											inputoldpass = infoDat.at(1);
											coord = infoDat.at(0).substr(16, infoDat.at(0).length() - 1).c_str();
											auto isdbox = std::experimental::filesystem::exists("save/safevault/_" + pData->currentWorld + "/X" + coord + ".json");
											if (!isdbox)
											{
												Player::OnTalkBubble(peer, pData->netID, "`4An error occured. Break the safe.", 0, true);
												break;
											}

											ifstream ifff("save/safevault/_" + pData->currentWorld + "/X" + coord + ".json");
											json j;
											ifff >> j;
											ifff.close();

											if (j["password"] != inputoldpass)
											{
												Player::OnTalkBubble(peer, pData->netID, "`4Old password did not match", 0, true);
												break;
											}

											j["password"] = "";
											j["recovery"] = "";
											ofstream of("save/safevault/_" + pData->currentWorld + "/X" + coord + ".json");
											of << j << std::endl;
											of.close();

											Player::OnTalkBubble(peer, pData->netID, "Your safe vault is no longer `4Password Protected`o!", 0, true);
										}
									}
								}
							}
							catch (const std::out_of_range& e) {
								std::cout << e.what() << std::endl;
							}
						}
						if (btn == "safe_setpassword")
						{
							if (world == nullptr || pData->currentWorld == "EXIT" || serverIsFrozen) continue;
							try {
								std::stringstream bss(cch);
								std::string bto;

								string inputnewpass = "";
								string coord = "";
								string inputnewrecoveryans = "";

								while (std::getline(bss, bto, '\n'))
								{
									vector<string> infoDat = explode("|", bto);
									if (infoDat.at(0).substr(0, 12) == "safenewpass_")
									{
										if (pData->rawName == PlayerDB::getProperName(world->owner) || world->owner == "" || mod(peer))
										{
											inputnewpass = infoDat.at(1);
											coord = infoDat.at(0).substr(12, infoDat.at(0).length() - 1).c_str();
											auto isdbox = std::experimental::filesystem::exists("save/safevault/_" + pData->currentWorld + "/X" + coord + ".json");
											if (!isdbox)
											{
												Player::OnTalkBubble(peer, pData->netID, "`4An error occured. Break the safe.", 0, true);
												break;
											}

											if (inputnewpass.size() < 2)
											{
												Player::OnTalkBubble(peer, pData->netID, "`4Your password is too easy.", 0, true);
												break;
											}
										}
									}
									if (infoDat.at(0) == "safe_recoverypass")
									{
										if (pData->rawName == PlayerDB::getProperName(world->owner) || world->owner == "" || mod(peer))
										{
											inputnewrecoveryans = infoDat.at(1);

											if (inputnewpass.size() < 2)
											{
												Player::OnTalkBubble(peer, pData->netID, "`4Your recovery answer is too easy.", 0, true);
												break;
											}
											ifstream ifff("save/safevault/_" + pData->currentWorld + "/X" + coord + ".json");
											json j;
											ifff >> j;
											ifff.close();
											j["password"] = inputnewpass;
											j["recovery"] = inputnewrecoveryans;
											ofstream of("save/safevault/_" + pData->currentWorld + "/X" + coord + ".json");
											of << j << std::endl;
											of.close();
											Player::OnTalkBubble(peer, pData->netID, "Your safe vault is now `2Password Protected`o!", 0, true);
										}
									}
								}
							}
							catch (const std::out_of_range& e) {
								std::cout << e.what() << std::endl;
							}
						}
						if (btn == "safe_confirmpass")
						{
							if (world == nullptr || pData->currentWorld == "EXIT" || serverIsFrozen) continue;
							try {
								std::stringstream bss(cch);
								std::string bto;

								string inputpass = "";

								while (std::getline(bss, bto, '\n'))
								{
									vector<string> infoDat = explode("|", bto);
									if (infoDat.at(0).substr(0, 21) == "safeconfirmpassInput_")
									{
										inputpass = infoDat.at(1);
										if (pData->rawName == PlayerDB::getProperName(world->owner) || world->owner == "" || mod(peer))
										{
											string coord = infoDat.at(0).substr(21, infoDat.at(0).length() - 1).c_str();
											auto isdbox = std::experimental::filesystem::exists("save/safevault/_" + pData->currentWorld + "/X" + coord + ".json");
											if (!isdbox)
											{
												Player::OnTalkBubble(peer, pData->netID, "`4An error occured. Break the safe.", 0, true);
												break;
											}

											ifstream ifff("save/safevault/_" + pData->currentWorld + "/X" + coord + ".json");
											json j;
											ifff >> j;
											ifff.close();
											string basic_string = j["password"];
											if (inputpass != basic_string)
											{
												Player::OnTalkBubble(peer, pData->netID, "`4Password did not match", 0, true);
												break;
											}
											int stored = j["insafe"];

											Player::OnTalkBubble(peer, pData->netID, "`2Password matched", 0, true);

											GTDialog mySafe;
											mySafe.addLabelWithIcon("`wSafe Vault", 8878, LABEL_BIG);

											if (stored > 0)
											{
												mySafe.addSpacer(SPACER_SMALL);
											}

											int count = 0;
											int id = 0;
											int aposition = 0;
											for (int i = 0; i < 20; i++)
											{
												if (j["safe"].at(i)["itemid"] != 0)
												{
													count = j["safe"].at(i)["itemcount"];
													id = j["safe"].at(i)["itemid"];
													aposition = j["safe"].at(i)["aposition"];

													if (i % 6 == 0 && i != 0)
													{
														mySafe.addStaticBlueFrameWithIdCountText(to_string(id), to_string(count), getItemDef(id).name, "safeBoxDepositedItem_" + to_string(aposition) + "_" + coord, true);
													}
													else
													{
														mySafe.addStaticBlueFrameWithIdCountText(to_string(id), to_string(count), getItemDef(id).name, "safeBoxDepositedItem_" + to_string(aposition) + "_" + coord, false);
													}
												}
											}

											if (stored > 0)
											{
												mySafe.addNewLineAfterFrame();
											}

											mySafe.addSmallText("`w" + to_string(stored) + "/20 `$items stored.");
											mySafe.addSpacer(SPACER_SMALL);
											mySafe.addPicker("safedeposit_" + coord, "Deposit item", "Select an item");
											if (j["password"] == "")
											{
												mySafe.addSmallText("`$This Safe Vault is not `4password protected`$!");
											}
											else
											{
												mySafe.addSmallText("`$This Safe Vault is `2password protected`$!");
											}
											mySafe.addSpacer(SPACER_SMALL);
											mySafe.addSmallText("`$Change your password.");
											mySafe.addButton("safeupdatepass_" + coord, "Update Password");

											mySafe.addSpacer(SPACER_SMALL);
											mySafe.addQuickExit();
											mySafe.endDialog("Close", "", "Exit");
											Player::OnDialogRequest(peer, mySafe.finishDialog());
											break;
										}
									}
								}
							}
							catch (const std::out_of_range& e) {
								std::cout << e.what() << std::endl;
							}
						}
						if (btn == "pickupdisplayitem")
						{
							if (world == nullptr || pData->currentWorld == "EXIT" || serverIsFrozen) continue;
							if (pData->rawName == world->owner || world->owner == "")
							{
								try {
									short dfg = pData->displayfg;
									short dbg = pData->displaybg;
									int x = pData->displaypunchx;
									int y = pData->displaypunchy;
									pData->displayfg = 0;
									pData->displaybg = 0;
									pData->displaypunchx = 0;
									pData->displaypunchy = 0;
									if (world->items.at(x + (y * world->width)).intdata != 0)
									{
										Player::OnTalkBubble(peer, pData->netID, "`wYou removed `5" + getItemDef(world->items.at(x + (y * world->width)).intdata).name + " `wfrom the Display Block.", 0, true);
										bool success = true;
										SaveItemMoreTimes(world->items.at(x + (y * world->width)).intdata, 1, peer, success, pData->rawName + " from Display Block");
										world->items.at(x + (y * world->width)).intdata = 0;
										ENetPeer* currentPeer;
										for (currentPeer = server->peers;
											currentPeer < &server->peers[server->peerCount];
											++currentPeer)
										{
											if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL)
												continue;
											if (isHere(peer, currentPeer))
											{
												UpdateVisualsForBlock(currentPeer, true, x, y, world);
											}
										}
									}
									else
									{
										Player::OnTalkBubble(peer, pData->netID, "Error occured while trying to remove item!", 0, true);
										continue;
									}
								}
								catch (const std::out_of_range& e) {
									std::cout << e.what() << std::endl;
								}
							}
						}
						if (btn == "WorldDropPickup") {
							if (world == nullptr || pData->currentWorld == "EXIT" || serverIsFrozen) continue;
							if (pData->rawName == world->owner || mod(peer)) {
								if (!world->DisableDrop) {
									world->DisableDrop = true;
									Player::OnConsoleMessage(peer, "`oVisitors can no longer collect dropped items!");
									for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
										if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
										if (pData->rawName == world->owner) continue;
										if (pData->currentWorld != "EXIT") {
											if (pData->rawName != static_cast<PlayerInfo*>(currentPeer->data)->rawName && pData->currentWorld == static_cast<PlayerInfo*>(currentPeer->data)->currentWorld) {
												Player::OnConsoleMessage(currentPeer, "`oOwner has disabled ability to collect dropped items!");
											}
										}
									}
								}
								else {
									world->DisableDrop = false;
									Player::OnConsoleMessage(peer, "`oVisitors can again collect dropped items!");
									for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
										if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
										if (pData->rawName == world->owner) continue;
										if (pData->currentWorld != "EXIT") {
											if (pData->rawName != static_cast<PlayerInfo*>(currentPeer->data)->rawName && pData->currentWorld == static_cast<PlayerInfo*>(currentPeer->data)->currentWorld) {
												Player::OnConsoleMessage(currentPeer, "`oOwner has enabled ability to collect dropped items!");
											}
										}
									}
								}
							}
						}
						if (btn == "getremote") {
							if (world == nullptr || pData->currentWorld == "EXIT" || serverIsFrozen) continue;
							try {
								int x = pData->lastPunchX;
								int y = pData->lastPunchY;
								bool canuseremote = true;
								if (canuseremote) {
									if (pData->inventory.items.size() == pData->currentInventorySize) {
										Player::OnTalkBubble(peer, pData->netID, "`wInventory is full!", 0, true);
										continue;
									}
									bool iscontainss = false;
									SearchInventoryItem(peer, 5640, 1, iscontainss);
									if (!iscontainss) {
										if (isWorldOwner(peer, world) && world->items.at(x + (y * world->width)).mid != 0 && world->items.at(x + (y * world->width)).mc != 0 || world->items.at(x + (y * world->width)).rm && world->items.at(x + (y * world->width)).mid != 0 && world->items.at(x + (y * world->width)).mc != 0) {
											Player::OnTalkBubble(peer, pData->netID, "`wYou received a MAGPLANT 5000 Remote.", 0, true);
											bool success = true;
											SaveItemMoreTimes(5640, 1, peer, success, pData->rawName + " from MAGPLANT 5000");
											pData->magplantitemid = world->items.at(x + (y * world->width)).mid;
											pData->magplantx = x;
											pData->magplanty = y;
										}
									}
									else {
										pData->magplantitemid = world->items.at(x + (y * world->width)).mid;
										pData->magplantx = x;
										pData->magplanty = y;
										Player::OnTalkBubble(peer, pData->netID, "`wYou received a MAGPLANT 5000 Remote.", 0, true);
									}
								}
							}
							catch (const std::out_of_range& e) {
								std::cout << e.what() << std::endl;
							}
						}
						if (btn == "retrieveitem") {
							if (world == nullptr || pData->currentWorld == "EXIT" || serverIsFrozen) continue;
							if (pData->rawName != world->owner && !mod(peer)) continue;
							try {
								int x = pData->lastPunchX;
								int y = pData->lastPunchY;
								int suckerid = pData->suckerid;
								string suckername = pData->suckername;
								if (world->items.at(x + (y * world->width)).mid == 112) {
									bool iscontainss = false;
									SearchInventoryItem(peer, 5640, 1, iscontainss);
									if (iscontainss) {
										pData->magplantitemid = 0;
										RemoveInventoryItem(5640, 1, peer, true);
									}
									int currentgems = 0;
									ifstream fs("save/gemdb/_" + pData->rawName + ".txt");
									fs >> currentgems;
									fs.close();
									currentgems += world->items.at(x + (y * world->width)).mc;
									Player::OnSetBux(peer, currentgems, 0);
									ofstream of("save/gemdb/_" + pData->rawName + ".txt");
									of << currentgems;
									of.close();
									Player::OnTalkBubble(peer, pData->netID, "`oCollected `2" + to_string(world->items.at(x + (y * world->width)).mc) + " `5" + getItemDef(world->items.at(x + (y * world->width)).mid).name + " `oFrom the " + getItemDef(suckerid).name + "!", 0, true);
									for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
										if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
										if (isHere(peer, currentPeer)) {
											send_item_sucker(currentPeer, suckerid, x, y, world->items.at(x + (y * world->width)).mid, 0, true, true, world->items.at(x + (y * world->width)).background);
										}
									}
									world->items.at(x + (y * world->width)).mc = 0;
									continue;
								}
								if (world->items.at(x + (y * world->width)).mc > 250) {
									if (CheckItemMaxed(peer, world->items.at(x + (y * world->width)).mid, 250)) {
										Player::OnTalkBubble(peer, pData->netID, "`wI dont have a free space to pickup `2250 `$" + getItemDef(world->items.at(x + (y * world->width)).mid).name + "`w!", 0, true);
										continue;
									}
									else {
										world->items.at(x + (y * world->width)).mc -= 250;
										bool success = true;
										SaveItemMoreTimes(world->items.at(x + (y * world->width)).mid, 250, peer, success, pData->rawName + " withdraw from MAGPLANT 5000");
										Player::OnTalkBubble(peer, pData->netID, "`oCollected `2250 `5" + getItemDef(world->items.at(x + (y * world->width)).mid).name + " `oFrom the " + getItemDef(suckerid).name + "!", 0, true);
										ENetPeer* currentPeer;
										for (currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer)
										{
											if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
											if (isHere(peer, currentPeer))
											{
												send_item_sucker(currentPeer, suckerid, x, y, world->items.at(x + (y * world->width)).mid, 1, true, true, world->items.at(x + (y * world->width)).background);
											}
										}
									}
								}
								else {
									if (CheckItemMaxed(peer, world->items.at(x + (y * world->width)).mid, world->items.at(x + (y * world->width)).mc)) {
										Player::OnTalkBubble(peer, pData->netID, "`wI dont have a free space to pickup `2" + to_string(world->items.at(x + (y * world->width)).mc) + " `$" + getItemDef(world->items.at(x + (y * world->width)).mid).name + "`w!", 0, true);
										continue;
									}
									else {
										bool iscontainss = false;
										SearchInventoryItem(peer, 5640, 1, iscontainss);
										if (iscontainss) {
											pData->magplantitemid = 0;
											RemoveInventoryItem(5640, 1, peer, true);
										}
										bool success = true;
										SaveItemMoreTimes(world->items.at(x + (y * world->width)).mid, world->items.at(x + (y * world->width)).mc, peer, success, pData->rawName + " withdraw from MAGPLANT 5000");
										Player::OnTalkBubble(peer, pData->netID, "`oCollected `2" + to_string(world->items.at(x + (y * world->width)).mc) + " `5" + getItemDef(world->items.at(x + (y * world->width)).mid).name + " `oFrom the " + getItemDef(suckerid).name + "!", 0, true);
										ENetPeer* currentPeer;
										for (currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
											if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
											if (isHere(peer, currentPeer)) {
												send_item_sucker(currentPeer, suckerid, x, y, world->items.at(x + (y * world->width)).mid, 0, true, true, world->items.at(x + (y * world->width)).background);
											}
										}
										world->items.at(x + (y * world->width)).mc = 0;
									}
								}
							}
							catch (const std::out_of_range& e) {
								std::cout << e.what() << std::endl;
							}
						}
						if (btn.substr(0, 17) == "showuseractivity_") {
							if (!mod(peer)) continue;
							string user = btn.substr(17, cch.length() - 17 - 1);
							try {
								ifstream read_player("save/players/_" + user + ".json");
								if (!read_player.is_open()) {
									Player::OnConsoleMessage(peer, user + " does not exist");
									continue;
								}
								json j;
								read_player >> j;
								read_player.close();
								string username = j["username"];
								string accountnotes = j["accountnotes"];
							}
							catch (std::exception& e) {
								std::cout << e.what() << std::endl;
								continue;
							}
						}
						if (btn.substr(0, 25) == "showuserinventoryoffline_") {
							if (!mod(peer)) continue;
							string user = btn.substr(25, cch.length() - 25 - 1);
							try {
								ifstream read_player("save/players/_" + user + ".json");
								if (!read_player.is_open()) {
									Player::OnConsoleMessage(peer, user + " does not exist");
									continue;
								}
								json j;
								read_player >> j;
								read_player.close();
								string username = j["username"];
								int inventorysize = j["inventorysize"];
								json inventory = j["inventory"];
								string appendItems = "";
								int itemid = 0;
								int itemCount = 0;
								for (int i = 0; i < inventorysize; i++) {
									itemid = inventory["items"].at(i)["itemid"];
									itemCount = inventory["items"].at(i)["quantity"];
									if (itemid == 0) continue;
									if (i % 6 == 0 && i != 0) {
										appendItems += "\nadd_button_with_icon||END_LIST|noflags|0|0|\nadd_button_with_icon|||staticBlueFrame|" + to_string(itemid) + "|" + to_string(itemCount) + "|";
									}
									else {
										appendItems += "\nadd_button_with_icon|||staticBlueFrame|" + to_string(itemid) + "|" + to_string(itemCount) + "|";
									}
								}
								Player::OnDialogRequest(peer, "set_default_color|\nadd_label|mid|`wViewing " + username + " `winventory|right|\nadd_spacer|small|" + appendItems + "|\nadd_button_with_icon||END_LIST|noflags|0|0|\nadd_button|punishview|Back|");
							}
							catch (std::exception& e) {
								std::cout << e.what() << std::endl;
							}
							catch (const std::out_of_range& e) {
								std::cout << e.what() << std::endl;
							}
						}
						if (btn == "warptosb")
						{
							if (pData->currentWorld == "EXIT")
							{
								continue;
							}
							if (pData->isCursed == true)
							{
								GamePacket p0 = packetEnd(appendString(appendString(createPacket(), "OnConsoleMessage"), "`4You are cursed now!"));
								ENetPacket* packet0 = enet_packet_create(p0.data,
									p0.len,
									ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet0);
								delete p0.data;
								continue;
							}
							string act = pData->lastSdbWorld;
							if (act == "")
							{
								GamePacket po = packetEnd(appendString(appendString(createPacket(), "OnTextOverlay"), "Unable to track down the location of the broadcast."));
								ENetPacket* packet = enet_packet_create(po.data,
									po.len,
									ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
							}
							else if (act == pData->currentWorld)
							{
								GamePacket po = packetEnd(appendString(appendString(createPacket(), "OnTextOverlay"), "Sorry, but you are already in the world!"));
								ENetPacket* packet = enet_packet_create(po.data,
									po.len,
									ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);
							}
							else
							{
								handle_world(peer, act);
							}
						}
						if (btn == "inviteguildbutton") {
							if (pData->guild == "") continue;
							try {
								ifstream read_guild("save/guilds/_" + pData->guild + ".json");
								if (!read_guild.is_open()) {
									continue;
								}
								json js;
								read_guild >> js;
								read_guild.close();
								string read_member = js["members"];
								vector<string> member_array = explode("|", read_member);
								if (member_array.size() > 10) {
									Player::OnConsoleMessage(peer, "`3GUILD ERROR: `oGuild is full capacity 10/10 members, cannot invite more players");
									continue;
								}
								for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
									if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
									if (isHere(peer, currentPeer)) {
										if (static_cast<PlayerInfo*>(currentPeer->data)->rawName == pData->lastInfo) {
											string name = static_cast<PlayerInfo*>(currentPeer->data)->rawName;
											if (static_cast<PlayerInfo*>(currentPeer->data)->guild != "") {
												Player::OnConsoleMessage(peer, "`3GUILD ERROR: `w" + static_cast<PlayerInfo*>(currentPeer->data)->displayName + "`o is already in a Guild!");
											}
											else {
												Player::OnTalkBubble(peer, pData->netID, "`5[`wGuild request sent to `2" + static_cast<PlayerInfo*>(currentPeer->data)->displayName + "`5]", 0, true);
												Player::PlayAudio(currentPeer, "audio/tip_start.wav", 0);
												static_cast<PlayerInfo*>(currentPeer->data)->guildlast = pData->guild;
												static_cast<PlayerInfo*>(currentPeer->data)->isinvited = true;
												Player::OnConsoleMessage(currentPeer, "`5[GUILD REQUEST] `oYou've been invited to join `2" + pData->guild + "`o by `w" + pData->displayName + "`o! To accept, `wwrench yourself `oand then choose `2Join " + pData->guild + "`o.");
											}
											break;
										}
									}
								}
							}
							catch (std::exception& e) {
								std::cout << e.what() << std::endl;
								break;
							}
						}
						if (btn == "manage_audio") {
							Player::OnDialogRequest(peer, "set_default_color|`o\nadd_label|big|Select a song|right|\nadd_label|small|Just a quick note there none of these songs are owned by us they are suggested by server players and were added by the developer's Once you start playing song there is no way to stop it!|left|\nadd_spacer|small|\nadd_checkbox|checkbox_ahrix|Ahrix Nova (Ahrix)|0|\nadd_checkbox|checkbox_phut|Phut Hon (Vietnam)|0|\nadd_checkbox|checkbox_stopme|Don't stop me now (QUEEN)|0|\nadd_checkbox|checkbox_feelit|Can you feel it (NCS)|0|\nadd_checkbox|checkbox_sayso|Say So! (MEP)|0|\nadd_checkbox|checkbox_bealone|Never be alone (Cursed Sheep)|0|\nadd_button|Cancel|Play|noflags|0|0|\nend_dialog|song_edit||");
						}
						if (btn == "change_password")
						{
							Player::OnDialogRequest(peer, "set_default_color|`o\n\nadd_label_with_icon|big|`oChange your password``|left|32|\nadd_textbox|`oEnter your old password, then enter a new password and confirm your new password. Your GrowID will be updated automatically, please remember `$not to share `oyour password, if you do so there is no way to recover lost data`o!|\nadd_spacer|small|\nadd_text_input|current_password|`$Current Password``||32|\nadd_text_input|new_password|`$New Password``||32|\nadd_text_input|confirm_new_password|`$Confirm New Password``||32|\nend_dialog|password_change|Cancel|Update|");
						}
						if (btn == "manage_title") {
							string titlecreatedialog = "", bluenametitle = "", titles = "No Titles Obtained", ltitle = "";
							if (pData->level >= 125) {
								titles = "Viewing your titles";
								if (pData->bluenametitle) {
									bluenametitle += "\nadd_checkbox|blue_name|Blue name title|1";
								}
								else {
									bluenametitle += "\nadd_checkbox|blue_name|Blue name title|0";
								}
							} if (pData->ltitleunlocked == true) {
								titles = "Viewing your titles";
								if (pData->ltitle) {
									ltitle += "\nadd_checkbox|legendary_title|Legendary title|1";
								}
								else {
									ltitle += "\nadd_checkbox|legendary_title|Legendary title|0";
								}
							} if (pData->Subscriber) {
								titlecreatedialog = "\nadd_button|edit_title|`oEdit Title``|";
							}
							Player::OnDialogRequest(peer, "set_default_color|`o\nadd_label|big|" + titles + "|right|\nadd_spacer|small|" + bluenametitle + ltitle + titlecreatedialog + "\nadd_button||OK|noflags|0|0|\nend_dialog|title_edit||");
						}
						if (btn == "notebook_edit")
						{
							Player::OnDialogRequest(peer, "set_default_color|`o\nadd_label|big|Notebook|left|0|\nadd_text_box_input|personal_note||" + pData->notebook + "|128|5|\nadd_spacer|small|\nadd_button|save|Save|noflags|0|0|\nadd_button|notebook_clear|Clear|noflags|0|0|\nadd_button|cancel|Cancel|noflags|0|0|\nend_dialog|notebook_edit||\nadd_quick_exit|");
						}
						if (btn == "worlds_view") {
							try {
								std::sort(pData->worldsowned.begin(), pData->worldsowned.end());
								auto it = std::unique(pData->worldsowned.begin(), pData->worldsowned.end());
								pData->worldsowned.erase(it, pData->worldsowned.end());
								string worlds_owned = "";
								for (int i = 0; i < pData->worldsowned.size(); i++) {
									if (i == pData->worldsowned.size() - 1) {
										worlds_owned += pData->worldsowned.at(i) + "";
									}
									else {
										worlds_owned += pData->worldsowned.at(i) + ", ";
									}
								}
								if (worlds_owned == "") worlds_owned = "You dont have any worlds locked!";
								Player::OnDialogRequest(peer, "set_default_color|`o\nadd_label|big|Worlds Locked|right\nadd_spacer|small|\nadd_textbox|" + worlds_owned + "``|left|\nadd_spacer|small|\nadd_button||Close|noflags|0|0|\nend_dialog|my_worlds||");
							}
							catch (const std::out_of_range& e) {
								std::cout << e.what() << std::endl;
							}
						}
						if (btn == "notebook_clear") {
							if (!pData->haveGrowId) continue;
							pData->notebook = "";
							Player::OnTalkBubble(peer, pData->netID, "`2Text cleared.", 0, true);
						}
						if (btn == "joinguild") {
							if (pData->guildlast == "" || !pData->isinvited) continue;
							try {
								ifstream read_guild("save/guilds/_" + pData->guildlast + ".json");
								if (!read_guild.is_open()) {
									continue;
								}
								json js;
								read_guild >> js;
								read_guild.close();
								string read_member = js["members"];
								string guild_name = js["guild_name"];
								int g_fg = js["foreground"];
								int g_bg = js["background"];
								pData->guildFg = g_fg;
								pData->guildBg = g_bg;
								pData->guild_rank = 0;
								pData->guild = guild_name;
								vector<string> member_array = explode("|", read_member);
								if (member_array.size() > 10) {
									continue;
								}
								member_array.push_back(pData->rawName);
								member_array.push_back("0"); /*rank*/
								string write_member = "";
								for (int i = 0; i < member_array.size(); i++) {
									write_member += member_array.at(i) + "|";
								}
								js["members"] = write_member;
								ofstream write_guild("save/guilds/_" + pData->guildlast + ".json");
								write_guild << js << std::endl;
								write_guild.close();
								for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
									if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
									if (static_cast<PlayerInfo*>(currentPeer->data)->guild == pData->guild) {
										if (isHere(peer, currentPeer)) {
											gamepacket_t p(0, pData->netID);
											p.Insert("OnGuildDataChanged");
											p.Insert(36095);
											p.Insert(63158650);
											p.Insert((65536 * pData->guildBg) + pData->guildFg);
											p.Insert(pData->guild_rank);
											p.CreatePacket(currentPeer);
										}
										Player::OnConsoleMessage(currentPeer, "`5[GUILD ALERT] `2" + pData->displayName + " `ojoined the guild!");
									}
								}
							}
							catch (std::exception& e) {
								std::cout << e.what() << std::endl;
								break;
							}
							catch (const std::out_of_range& e) {
								std::cout << e.what() << std::endl;
							}
						}
						if (btn == "showguild") {
							string onlinegmlist = "";
							string grole = "";
							int onlinecount = 0;
							ifstream read_guild("save/guilds/_" + pData->guild + ".json");
							if (read_guild.fail() || pData->guild == "") {
								read_guild.close();
								Player::OnDialogRequest(peer, "set_default_color|`o\nadd_label_with_icon|big|`wGrow Guild ``|left|242|\nadd_textbox|Welcome to Grow Guilds where you can create a Guild! With a Guild you can compete in Guild Leaderboards to earn rewards and level up the Guild to add more members.|left|\nadd_spacer|small|\nadd_textbox|To create a Guild you must either be `2level 10 `oor a `2Subscriber``.|left|\nadd_textbox|The cost will be: `6100,000 `oGems.|left|\nadd_spacer|small|\nadd_textbox|`8Caution``: A guild can only be created in a world owned by you and locked with a `5World Lock``!|left|\nadd_spacer|small|\nadd_button|showcreateguild|Create a Guild|noflags|0|0|\nadd_button|back|Back|noflags|0|0|\nend_dialog|requestcreateguildpage|Close||");
								continue;
							}
							json j;
							read_guild >> j;
							read_guild.close();
							Player::OnTextOverlay(peer, "Coming Soon...");
						}
						if (btn == "ipbanwrench") {
							if (mod(peer)) {
								Player::OnTextOverlay(peer, "IP Ban mod applied to " + pData->lastInfo + "!");
								for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
									if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
									if (static_cast<PlayerInfo*>(currentPeer->data)->rawName == pData->lastInfo) {
										if (static_cast<PlayerInfo*>(currentPeer->data)->haveGrowId == false) continue;
										autoBan(currentPeer, true, 100000, "ip ban wrench");
									}
								}
								SendPunishView(peer, pData->lastInfo);
							}
						}
						if (btn == "banpanel") {
							if (mod(peer)) {
								pData->last_ban_days = 0;
								pData->last_ban_hours = 0;
								pData->last_ban_minutes = 0;
								pData->last_ban_reason = "";
								send_ban_panel(peer, "");
							}
						}
						if (btn == "mutepanel") {
							if (mod(peer)) {
								pData->last_ban_days = 0;
								pData->last_ban_hours = 0;
								pData->last_ban_minutes = 0;
								pData->last_ban_reason = "";
								send_mute_panel(peer, "");
							}
						}
						if (btn == "cursepanel") {
							if (mod(peer)) {
								pData->last_ban_days = 0;
								pData->last_ban_hours = 0;
								pData->last_ban_minutes = 0;
								pData->last_ban_reason = "";
								send_curse_panel(peer, "");
							}
						}
						if (btn == "suspend")
						{
							if (mod(peer))
							{
								Player::OnTextOverlay(peer, "Ban mod applied to " + pData->lastInfo + "!");
								GamePacket p7 = packetEnd(appendString(appendString(createPacket(), "OnConsoleMessage"), "`#** `$The Ancient Ones `ohave `4banned `w" + pData->lastInfo + " `#** `o(`4/rules `oto see the rules!)"));
								ENetPeer* currentPeer;
								time_t now = time(nullptr);
								char* dt = ctime(&now);
								for (currentPeer = server->peers;
									currentPeer < &server->peers[server->peerCount];
									++currentPeer)
								{
									if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL)
										continue;
									if (static_cast<PlayerInfo*>(currentPeer->data)->rawName == pData->lastInfo)
									{
										if (static_cast<PlayerInfo*>(currentPeer->data)->haveGrowId == false) continue;
										GamePacket ps2 = packetEnd(appendInt(appendString(appendString(appendString(appendString(createPacket(), "OnAddNotification"), "interface/atomic_button.rttex"), "`0Warning from `4System`0: You've been `4BANNED `0from GrowtopiaOS for 730 days"), "audio/hub_open.wav"), 0));
										ENetPacket* packet2 = enet_packet_create(ps2.data,
											ps2.len,
											ENET_PACKET_FLAG_RELIABLE);
										enet_peer_send(currentPeer, 0, packet2);
										GamePacket ps3 = packetEnd(appendString(appendString(createPacket(), "OnConsoleMessage"), "`oWarning from `4System`o: You've been `4BANNED `ofrom GrowtopiaOS for 730 days"));
										ENetPacket* packet3 = enet_packet_create(ps3.data,
											ps3.len,
											ENET_PACKET_FLAG_RELIABLE);
										enet_peer_send(currentPeer, 0, packet3);
										GamePacket pto = packetEnd(appendString(appendString(createPacket(), "OnTextOverlay"), "Applied punishment on " + pData->lastInfo + "."));
										ENetPacket* packetto = enet_packet_create(pto.data,
											pto.len,
											ENET_PACKET_FLAG_RELIABLE);
										enet_peer_send(peer, 0, packetto);
										enet_peer_disconnect_later(currentPeer, 0);
									}
									ENetPacket* packet7 = enet_packet_create(p7.data,
										p7.len,
										ENET_PACKET_FLAG_RELIABLE);
									enet_peer_send(currentPeer, 0, packet7);
								}
								string username = PlayerDB::getProperName(pData->rawName);
								try {
									ifstream read_player("save/players/_" + pData->lastInfo + ".json");
									if (!read_player.is_open()) {
										continue;
									}
									json j;
									read_player >> j;
									read_player.close();
									j["isBanned"] = true;
									ofstream write_player("save/players/_" + pData->lastInfo + ".json");
									write_player << j << std::endl;
									write_player.close();
								}
								catch (std::exception& e) {
									std::cout << e.what() << std::endl;
									continue;
								}
								SendPunishView(peer, pData->lastInfo);
							}
						}
						if (btn == "fakeautoban") {
							if (mod(peer)) {
								bool Found = false;
								Player::OnTextOverlay(peer, "Fake ban works only if player is online!");
								string userdisplay = "";
								for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
									if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
									if (static_cast<PlayerInfo*>(currentPeer->data)->rawName == pData->lastInfo) {
										Found = true;
										userdisplay = static_cast<PlayerInfo*>(currentPeer->data)->displayName;
										autoBan(currentPeer, true, 0, "");
										break;
									}
								}
								if (userdisplay == "") userdisplay = pData->lastInfo;
								if (Found) {
									for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
										if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
										Player::OnConsoleMessage(currentPeer, "`#** `$The Ancients `ohave used `#Ban `oon `w" + userdisplay + "`o! `#**");
									}
								}
								SendPunishView(peer, pData->lastInfo);
							}
						}
						if (btn == "punishview")
						{
							if (mod(peer))
								SendPunishView(peer, pData->lastInfo);
						}
						if (btn == "wban")
						{
							if (world == nullptr || pData->currentWorld == "EXIT" || serverIsFrozen) continue;
							if (pData->lastInfo == pData->rawName)
							{
								continue;
							}
							if (pData->haveGrowId && pData->rawName == world->owner || mod(peer))
							{
								string name = static_cast<PlayerInfo*>(peer->data)->displayName;
								string kickname = static_cast<PlayerInfo*>(peer->data)->rawName;
								ENetPeer* currentPeerp;
								for (currentPeerp = server->peers;
									currentPeerp < &server->peers[server->peerCount];
									++currentPeerp)
								{
									if (currentPeerp->state != ENET_PEER_STATE_CONNECTED || currentPeerp->data == NULL) continue;
									if (static_cast<PlayerInfo*>(currentPeerp->data)->rawName == kickname)
									{
										if (pData->rawName == world->owner || mod(peer))
										{
											Player::OnConsoleMessage(peer, "`4You can't ban world Owner or Moderators and above!");
											break;
										}
										else
										{
											GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnConsoleMessage"), name + " `4world bans " + "`o" + kickname + " from `w" + world->name + "`o!"));
											string text = "action|play_sfx\nfile|audio/repair.wav\ndelayMS|0\n";
											BYTE* data = new BYTE[5 + text.length()];
											BYTE zero = 0;
											int type = 3;
											memcpy(data, &type, 4);
											memcpy(data + 4, text.c_str(), text.length());
											memcpy(data + 4 + text.length(), &zero, 1);
											if (isHere(peer, currentPeerp))
											{
												ENetPacket* packetsou = enet_packet_create(data,
													5 + text.length(),
													ENET_PACKET_FLAG_RELIABLE);
												ENetPacket* packet = enet_packet_create(p.data,
													p.len,
													ENET_PACKET_FLAG_RELIABLE);
												enet_peer_send(currentPeerp, 0, packetsou);
												enet_peer_send(currentPeerp, 0, packet);
												delete[] data;
												delete p.data;
												if (static_cast<PlayerInfo*>(currentPeerp->data)->rawName == pData->lastInfo) // if last wrench
												{
													namespace fs = std::experimental::filesystem;
													if (!fs::is_directory("save/worldbans/_" + world->name) || !fs::exists("save/worldbans/_" + world->name))
													{
														fs::create_directory("save/worldbans/_" + world->name);
														std::ofstream outfile("save/worldbans/_" + world->name + "/" + static_cast<PlayerInfo*>(currentPeerp->data)->rawName);
														outfile << "worldbanned by: " + pData->rawName;
														outfile.close();
													}
													else
													{
														std::ofstream outfile("save/worldbans/_" + world->name + "/" + static_cast<PlayerInfo*>(currentPeerp->data)->rawName);
														outfile << "worldbanned by: " + pData->rawName;
														outfile.close();
													}
													sendPlayerLeave(currentPeerp);
													static_cast<PlayerInfo*>(currentPeerp->data)->currentWorld = "EXIT";
													sendWorldOffers(currentPeerp);
												}
											}
										}
									}
								}
							}
						}
						if (btn == "kick")
						{
							if (world == nullptr || pData->currentWorld == "EXIT" || serverIsFrozen) continue;
							if (pData->lastInfo == pData->rawName) continue;
							for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer)
							{
								if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
								if (isHere(peer, currentPeer))
								{
									if (pData->rawName == world->owner || mod(peer) || !restricted_area(peer, world, static_cast<PlayerInfo*>(currentPeer->data)->x / 32, static_cast<PlayerInfo*>(currentPeer->data)->y / 32) && world->owner == "")
									{
										string name = static_cast<PlayerInfo*>(peer->data)->displayName;
										string kickname = static_cast<PlayerInfo*>(peer->data)->rawName;
										ENetPeer* currentPeerp;
										for (currentPeerp = server->peers;
											currentPeerp < &server->peers[server->peerCount];
											++currentPeerp)
										{
											if (currentPeerp->state != ENET_PEER_STATE_CONNECTED || currentPeerp->data == NULL) continue;
											if (static_cast<PlayerInfo*>(currentPeerp->data)->rawName == kickname)
											{
												if (pData->rawName == world->owner || mod(peer))
												{
													Player::OnConsoleMessage(peer, "`4You can't kick world Owner or Moderators and above!");
													break;
												}
												else
												{
													Player::OnConsoleMessage(currentPeer, pData->displayName + " `4kicks " + "`o" + pData->lastDisplayname + "`o!");
													if (pData->lastInfo == static_cast<PlayerInfo*>(currentPeer->data)->rawName) playerRespawn(world, currentPeer, false);
												}
											}
										}
									}
								}
							}
						}
						if (btn == "pull")
						{
							if (world == nullptr || pData->currentWorld == "EXIT" || serverIsFrozen) continue;
							if (pData->lastInfo == pData->rawName)
							{
								continue;
							}
							if (pData->rawName == world->owner || isWorldAdmin(peer, world) || mod(peer))
							{
								ENetPeer* currentPeer;
								for (currentPeer = server->peers;
									currentPeer < &server->peers[server->peerCount];
									++currentPeer)
								{
									if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
									string name = static_cast<PlayerInfo*>(currentPeer->data)->rawName;
									int pullX = pData->x;
									int pullY = pData->y;
									if (name == pData->lastInfo)
									{
										if (isHere(peer, currentPeer) && world->name != "EXIT")
										{
											string name = pData->displayName;
											string pullname = static_cast<PlayerInfo*>(currentPeer->data)->displayName;
											GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnConsoleMessage"), name + " `5pulls " + "`o" + pullname + "`o!"));
											string text = "action|play_sfx\nfile|audio/object_spawn.wav\ndelayMS|0\n";
											BYTE* data = new BYTE[5 + text.length()];
											BYTE zero = 0;
											int type = 3;
											memcpy(data, &type, 4);
											memcpy(data + 4, text.c_str(), text.length());
											memcpy(data + 4 + text.length(), &zero, 1);
											ENetPacket* packet = enet_packet_create(p.data,
												p.len,
												ENET_PACKET_FLAG_RELIABLE);
											enet_peer_send(currentPeer, 0, packet);
											enet_peer_send(peer, 0, packet);
											ENetPacket* packetsou = enet_packet_create(data,
												5 + text.length(),
												ENET_PACKET_FLAG_RELIABLE);
											enet_peer_send(currentPeer, 0, packetsou);
											enet_peer_send(peer, 0, packetsou);
											GamePacket p2 = packetEnd(appendFloat(appendString(createPacket(), "OnSetPos"), pullX, pullY));
											memcpy(p2.data + 8, &(static_cast<PlayerInfo*>(currentPeer->data)->netID), 4);
											ENetPacket* packet2 = enet_packet_create(p2.data,
												p2.len,
												ENET_PACKET_FLAG_RELIABLE);
											enet_peer_send(currentPeer, 0, packet2);
											if (isWorldOwner(peer, world)) Player::OnTextOverlay(currentPeer, "You were pulled by " + pData->displayName + "");
											else if (mod(peer)) Player::OnTextOverlay(currentPeer, "You were summoned by a mod");
											delete p2.data;
											delete p.data;
										}
									}
								}
							}
						}
						if (btn == "backsocialportal") {
							Player::OnDialogRequest(peer, "set_default_color|`o\nadd_label_with_icon|big| `wSocial Portal`` |left|1366|\nadd_spacer|small|\nadd_button|showfriend|`wShow Friends``|noflags|0|0|\nadd_quick_exit|\nend_dialog|friends_guilds|OK||");
						}
						if (btn == "showfriend")
						{
							if (pData->currentWorld == "EXIT")
							{
								continue;
							}
							string onlinefrnlist = "";
							int onlinecount = 0;
							int totalcount = pData->friendinfo.size();
							ENetPeer* currentPeer;
							for (currentPeer = server->peers;
								currentPeer < &server->peers[server->peerCount];
								++currentPeer)
							{
								if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL)
									continue;
								string name = static_cast<PlayerInfo*>(currentPeer->data)->rawName;
								if (find(pData->friendinfo.begin(), pData->friendinfo.end(), name) != pData->friendinfo.end())
								{
									onlinefrnlist += "\nadd_button|onlinefrns_" + static_cast<PlayerInfo*>(currentPeer->data)->rawName + "|`2ONLINE: `o" + static_cast<PlayerInfo*>(currentPeer->data)->tankIDName + "``|0|0|";
									onlinecount++;
								}
							}
							if (totalcount == 0)
							{
								GamePacket p2 = packetEnd(appendString(appendString(createPacket(), "OnDialogRequest"), "set_default_color|`o\n\nadd_label_with_icon|big|`o" + std::to_string(onlinecount) + " of " + std::to_string(totalcount) + " `wFriends Online``|left|1366|\n\nadd_spacer|small|\nadd_label|small|`oYou currently have no friends. That's just sad. To make some, press a person's wrench icon, then choose `5Add as friend`o.``|left|4|\nadd_button|friendoptions|`oOptions``|left|4|\nadd_button|backsocialportal|`oBack``|0|0|\nadd_quick_exit|"));
								ENetPacket* packet2 = enet_packet_create(p2.data,
									p2.len,
									ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet2);
								delete p2.data;
							}
							else if (onlinecount == 0)
							{
								GamePacket p2 = packetEnd(appendString(appendString(createPacket(), "OnDialogRequest"), "set_default_color|`o\n\nadd_label_with_icon|big|`o" + std::to_string(onlinecount) + " of " + std::to_string(totalcount) + " `wFriends Online``|left|1366|\n\nadd_spacer|small|\nadd_button|chc0|`oClose``|0|0|\nadd_label|small|`oNone of your friends are currently online.``|left|4|\n\nadd_spacer|small|\nadd_button|showoffline|`oShow offline``|0|0|\nadd_button|backsocialportal|`oBack``|0|0|\nadd_quick_exit|"));
								ENetPacket* packet2 = enet_packet_create(p2.data,
									p2.len,
									ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet2);
								delete p2.data;
							}
							else
							{
								GamePacket p2 = packetEnd(appendString(appendString(createPacket(), "OnDialogRequest"), "set_default_color|`o\n\nadd_label_with_icon|big|`o" + std::to_string(onlinecount) + " of " + std::to_string(totalcount) + " `wFriends Online``|left|1366|\n\nadd_spacer|small|\nadd_button|chc0|`wClose``|0|0|" + onlinefrnlist + "\n\nadd_spacer|small|\nadd_button|showoffline|`oShow offline``|0|0|\nadd_button|backsocialportal|`oBack``|0|0|\nadd_quick_exit|"));
								ENetPacket* packet2 = enet_packet_create(p2.data,
									p2.len,
									ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet2);
								delete p2.data;
							}
						}
						if (btn.substr(0, 18) == "clearaccountnotes_") {
							if (std::find(creators.begin(), creators.end(), static_cast<PlayerInfo*>(peer->data)->rawName) != creators.end()) {
								string username = btn.substr(18, cch.length() - 18 - 1);
								try {
									ifstream read_player("save/players/_" + username + ".json");
									if (!read_player.is_open()) {
										continue;
									}
									json j;
									read_player >> j;
									read_player.close();
									j["accountnotes"] = "";
									ofstream write_player("save/players/_" + username + ".json");
									write_player << j << std::endl;
									write_player.close();
								}
								catch (std::exception& e) {
									std::cout << e.what() << std::endl;
									continue;
								}
								Player::OnTextOverlay(peer, "Account notes cleared for " + username + "!");
								SendPunishView(peer, username);
							}
						}
						if (btn.substr(0, 7) == "warpto_") {
							if (mod(peer)) handle_world(peer, btn.substr(7, cch.length() - 7 - 1));
						}
						if (btn.substr(0, 9) == "viewinfo_") {
							if (mod(peer)) SendPunishView(peer, btn.substr(9, cch.length() - 9 - 1));
						}
						if (btn.substr(0, 9) == "onlinegm_")
						{
							if (pData->currentWorld == "EXIT")
							{
								continue;
							}
							ENetPeer* currentPeer;
							for (currentPeer = server->peers;
								currentPeer < &server->peers[server->peerCount];
								++currentPeer)
							{
								if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL)
									continue;
								if (static_cast<PlayerInfo*>(currentPeer->data)->rawName == btn.substr(9, cch.length() - 9 - 1))
								{
									pData->lastgmworld = static_cast<PlayerInfo*>(currentPeer->data)->currentWorld;
									pData->lastgmname = static_cast<PlayerInfo*>(currentPeer->data)->displayName;
									pData->lastgm = static_cast<PlayerInfo*>(currentPeer->data)->rawName;
								}
							}
							if (btn.substr(9, cch.length() - 9 - 1) == pData->rawName)
							{
								GamePacket p2 = packetEnd(appendString(appendString(createPacket(), "OnDialogRequest"), "set_default_color|`o\n\nadd_label_with_icon|big|`w" + pData->lastgmname + "``|left|1366|\n\nadd_spacer|small|\nadd_textbox|This is you!|\n\nadd_spacer|small|\nadd_button|showguild|`oBack``|0|0|\nadd_quick_exit|"));
								ENetPacket* packet2 = enet_packet_create(p2.data,
									p2.len,
									ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet2);
								delete p2.data;
							}
							else
							{
								if (pData->rawName == pData->guildLeader)
								{
									GamePacket p2 = packetEnd(appendString(appendString(createPacket(), "OnDialogRequest"), "set_default_color|`o\n\nadd_label_with_icon|big|`w" + pData->lastgmname + "``|left|1366|\n\nadd_spacer|small|\nadd_textbox|`o" + pData->lastgmname + " is `2online `onow in the world `5" + pData->lastgmworld + "`o.|\n\nadd_spacer|small|\nadd_button|gmwarpbutton|`oWarp to `5" + pData->lastgmworld + "``|0|0|\nadd_button|gmmsgbutton|`5Send message``|0|0|\n\nadd_spacer|small|\nadd_button|removegmonline|Kick from guild|0|0|\nadd_button|showguild|`oBack``|0|0|\nadd_quick_exit|"));
									ENetPacket* packet2 = enet_packet_create(p2.data,
										p2.len,
										ENET_PACKET_FLAG_RELIABLE);
									enet_peer_send(peer, 0, packet2);
									delete p2.data;
								}
								else
								{
									GamePacket p2 = packetEnd(appendString(appendString(createPacket(), "OnDialogRequest"), "set_default_color|`o\n\nadd_label_with_icon|big|`w" + pData->lastgmname + "``|left|1366|\n\nadd_spacer|small|\nadd_textbox|`o" + pData->lastgmname + " is `2online `onow in the world `5" + pData->lastgmworld + "`o.|\n\nadd_spacer|small|\nadd_button|gmwarpbutton|`oWarp to `5" + pData->lastgmworld + "``|0|0|\nadd_button|gmmsgbutton|`5Send message``|0|0|\n\nadd_spacer|small|\nadd_button|showguild|`oBack``|0|0|\nadd_quick_exit|"));
									ENetPacket* packet2 = enet_packet_create(p2.data,
										p2.len,
										ENET_PACKET_FLAG_RELIABLE);
									enet_peer_send(peer, 0, packet2);
									delete p2.data;
								}
							}
						}
						if (btn == "showoffline")
						{
							if (pData->currentWorld == "EXIT")
							{
								continue;
							}
							string onlinelist = "";
							string offlinelist = "";
							string offname = "";
							int onlinecount = 0;
							int totalcount = pData->friendinfo.size();
							vector<string> offliness = pData->friendinfo;
							ENetPeer* currentPeer;
							for (currentPeer = server->peers;
								currentPeer < &server->peers[server->peerCount];
								++currentPeer)
							{
								if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL)
									continue;
								string name = static_cast<PlayerInfo*>(currentPeer->data)->rawName;
								if (find(pData->friendinfo.begin(), pData->friendinfo.end(), name) != pData->friendinfo.end())
								{
									onlinelist += "\nadd_button|onlinefrns_" + static_cast<PlayerInfo*>(currentPeer->data)->rawName + "|`2ONLINE: `o" + static_cast<PlayerInfo*>(currentPeer->data)->displayName + "``|0|0|";
									onlinecount++;
									offliness.erase(std::remove(offliness.begin(), offliness.end(), name), offliness.end());
								}
							}
							for (std::vector<string>::const_iterator i = offliness.begin(); i != offliness.end(); ++i)
							{
								offname = *i;
								offlinelist += "\nadd_button|offlinefrns_" + offname + "|`4OFFLINE: `o" + offname + "``|0|0|";
							}
							GamePacket p2 = packetEnd(appendString(appendString(createPacket(), "OnDialogRequest"), "set_default_color|`o\n\nadd_label_with_icon|big|`o" + std::to_string(onlinecount) + " of " + std::to_string(totalcount) + " `wFriends Online|left|1366|\n\nadd_spacer|small|\nadd_button|chc0|`wClose``|0|0|\nadd_spacer|small|" + offlinelist + "\nadd_spacer|small|\n\nadd_button|frnoption|`oFriend Options``|0|0|\nadd_button|showfriend|Back``|0|0|\nadd_button||`oClose``|0|0|\nadd_quick_exit|"));
							ENetPacket* packet2 = enet_packet_create(p2.data,
								p2.len,
								ENET_PACKET_FLAG_RELIABLE);
							enet_peer_send(peer, 0, packet2);
							delete p2.data;
						}
						if (btn == "removecon")
						{
							if (pData->currentWorld == "EXIT" || pData->haveGrowId == false)
							{
								continue;
							}
							try {
								for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
									if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
									if (static_cast<PlayerInfo*>(currentPeer->data)->rawName == pData->lastFrn) {
										pData->friendinfo.erase(std::remove(pData->friendinfo.begin(), pData->friendinfo.end(), pData->lastFrn), pData->friendinfo.end());
										static_cast<PlayerInfo*>(currentPeer->data)->friendinfo.erase(std::remove(static_cast<PlayerInfo*>(currentPeer->data)->friendinfo.begin(), static_cast<PlayerInfo*>(currentPeer->data)->friendinfo.end(), pData->rawName), static_cast<PlayerInfo*>(currentPeer->data)->friendinfo.end());
										string friends_string = "";
										for (int i = 0; i < pData->friendinfo.size(); i++) {
											friends_string += pData->friendinfo.at(i) + ",";
										}
										ifstream read_player("save/players/_" + pData->rawName + ".json");
										if (!read_player.is_open()) {
											continue;
										}
										json j;
										read_player >> j;
										read_player.close();
										j["friends"] = friends_string;
										ofstream write_player("save/players/_" + pData->rawName + ".json");
										write_player << j << std::endl;
										write_player.close();
										string friends_string2 = "";
										for (int i = 0; i < static_cast<PlayerInfo*>(currentPeer->data)->friendinfo.size(); i++) {
											friends_string2 += static_cast<PlayerInfo*>(currentPeer->data)->friendinfo.at(i) + ",";
										}
										ifstream read_player3("save/players/_" + static_cast<PlayerInfo*>(currentPeer->data)->rawName + ".json");
										if (!read_player3.is_open()) {
											continue;
										}
										json j3;
										read_player3 >> j3;
										read_player3.close();
										j3["friends"] = friends_string2;
										ofstream write_player3("save/players/_" + static_cast<PlayerInfo*>(currentPeer->data)->rawName + ".json");
										write_player3 << j3 << std::endl;
										write_player3.close();
										Player::OnConsoleMessage(currentPeer, "`3FRIEND ALERT: `2" + pData->displayName + " `ohas removed you as a friend.");
										Player::OnDialogRequest(peer, "set_default_color|`o\n\nadd_label_with_icon|big|`4Friend removed``|left|1366|\n\nadd_spacer|small|\nadd_textbox|`oOk, you are no longer friends with `o" + pData->lastFrnName + ".``|\n\nadd_spacer|small|\nadd_button||`oOK``|0|0|\nadd_quick_exit|");
										break;
									}
								}
							}
							catch (const std::out_of_range& e) {
								std::cout << e.what() << std::endl;
							}
							catch (std::exception& e) {
								std::cout << e.what() << std::endl;
							}
						}
						if (btn == "removeconoff") {
							if (pData->currentWorld == "EXIT") {
								continue;
							}
							if (pData->haveGrowId == false) {
								continue;
							}
							try {
								pData->friendinfo.erase(std::remove(pData->friendinfo.begin(), pData->friendinfo.end(), pData->lastFrn), pData->friendinfo.end());
								ifstream read_player("save/players/_" + pData->lastFrn + ".json");
								if (!read_player.is_open()) {
									continue;
								}
								json j;
								read_player >> j;
								read_player.close();
								string Friends = j["friends"];
								stringstream ssf(Friends);
								vector<string> result;
								while (ssf.good()) {
									string substr;
									getline(ssf, substr, ',');
									if (substr.size() == 0) continue;
									result.push_back(substr);
								}
								result.erase(std::remove(result.begin(), result.end(), pData->rawName), result.end());
								string friends_string = "";
								for (int i = 0; i < result.size(); i++) {
									friends_string += result.at(i) + ",";
								}
								j["friends"] = friends_string;
								ofstream write_player("save/players/_" + pData->lastFrn + ".json");
								write_player << j << std::endl;
								write_player.close();
								ifstream read_player3("save/players/_" + pData->rawName + ".json");
								if (!read_player3.is_open()) {
									continue;
								}
								json j3;
								read_player3 >> j3;
								read_player3.close();
								string friends_string3 = "";
								for (int i = 0; i < pData->friendinfo.size(); i++) {
									friends_string3 += pData->friendinfo.at(i) + ",";
								}
								j3["friends"] = friends_string3;
								ofstream write_player3("save/players/_" + pData->rawName + ".json");
								write_player3 << j3 << std::endl;
								write_player3.close();
								Player::OnDialogRequest(peer, "set_default_color|`o\n\nadd_label_with_icon|big|`4Friend removed``|left|1366|\n\nadd_spacer|small|\nadd_textbox|`oOk, you are no longer friends with `o" + pData->lastFrn + ".``|\n\nadd_spacer|small|\nadd_button||`oOK``|0|0|\nadd_quick_exit|");
							}
							catch (const std::out_of_range& e) {
								std::cout << e.what() << std::endl;
							}
							catch (std::exception& e) {
								std::cout << e.what() << std::endl;
							}
						}
						if (btn.substr(0, 11) == "onlinefrns_")
						{
							if (pData->currentWorld == "EXIT")
							{
								continue;
							}
							ENetPeer* currentPeer;
							for (currentPeer = server->peers;
								currentPeer < &server->peers[server->peerCount];
								++currentPeer)
							{
								if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL)
									continue;
								if (static_cast<PlayerInfo*>(currentPeer->data)->rawName == btn.substr(11, cch.length() - 11 - 1))
								{
									pData->lastFrnWorld = static_cast<PlayerInfo*>(currentPeer->data)->currentWorld;
									pData->lastFrnName = static_cast<PlayerInfo*>(currentPeer->data)->tankIDName;
									pData->lastFrn = static_cast<PlayerInfo*>(currentPeer->data)->rawName;
								}
							}
							ifstream read_player("save/players/_" + pData->rawName + ".json");
							if (!read_player.is_open()) {
								continue;
							}
							json j;
							read_player >> j;
							read_player.close();
							j["showlocation"];
							if (j = true)
							{
								GamePacket p2 = packetEnd(appendString(appendString(createPacket(), "OnDialogRequest"), "set_default_color|`o\n\nadd_label_with_icon|big|`w" + pData->lastFrnName + "``|left|1366|\n\nadd_spacer|small|\nadd_textbox|`o" + pData->lastFrnName + " is `2online `onow in the world `5" + pData->lastFrnWorld + "`o.|\n\nadd_spacer|small|\nadd_button|frnwarpbutton|`oWarp to `5" + pData->lastFrnWorld + "``|0|0|\nadd_button|msgbutton|`5Send message``|0|0|\n\nadd_spacer|small|\nadd_button|removecon|`oRemove as friend``|0|0|\nadd_button|showfriend|`oBack``|0|0|\nadd_quick_exit|"));
								ENetPacket* packet2 = enet_packet_create(p2.data,
									p2.len,
									ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet2);
								delete p2.data;
							}
							else
							{
								GamePacket p3 = packetEnd(appendString(appendString(createPacket(), "OnDialogRequest"), "set_default_color|`o\n\nadd_label_with_icon|big|`w" + pData->lastFrnName + "``|left|1366|\n\nadd_spacer|small|\nadd_textbox|`othis person disabled show location!|0|0|\nadd_button|msgbutton|`5Send message``|0|0|\n\nadd_spacer|small|\nadd_button|removecon|`oRemove as friend``|0|0|\nadd_button|showfriend|`oBack``|0|0|\nadd_quick_exit|"));
								ENetPacket* packet3 = enet_packet_create(p3.data,
									p3.len,
									ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet3);
								delete p3.data;
							}
						}
						if (btn == "abandon_guild") {
							if (pData->guild == "" || !isWorldOwner(peer, world) && !admin(peer)) continue;
							Player::OnDialogRequest(peer, "set_default_color|`o\nadd_label_with_icon|big|`wGrow Guild ``|left|242|\nadd_textbox|`oAre you sure you want to abandon " + pData->guild + "? Confirming this will delete your guild from existence, you and your guild members will be kicked out from the guild!|left|\nend_dialog|abandon_guild|Close|Confirm|");
						}
						if (btn == "msgbutton")
						{
							if (pData->currentWorld == "EXIT")
							{
								continue;
							}
							GamePacket p2 = packetEnd(appendString(appendString(createPacket(), "OnDialogRequest"), "add_label_with_icon|big|`5Message to `o" + pData->lastFrnName + "|left|660|\nadd_spacer|small|\nadd_text_input|msgtext|||50|\nend_dialog|msgdia|Cancel|`5Send``| \nadd_spacer|big|\nadd_button|showfriend|`oBack``|0|0|\nadd_quick_exit|\n"));
							ENetPacket* packet2 = enet_packet_create(p2.data,
								p2.len,
								ENET_PACKET_FLAG_RELIABLE);
							enet_peer_send(peer, 0, packet2);
							delete p2.data;
						}
						if (btn.substr(0, 12) == "offlinefrns_")
						{
							if (pData->currentWorld == "EXIT")
							{
								continue;
							}
							pData->lastFrn = btn.substr(12, cch.length() - 12 - 1);
							GamePacket p2 = packetEnd(appendString(appendString(createPacket(), "OnDialogRequest"), "set_default_color|`o\n\nadd_label_with_icon|big|`w" + pData->lastFrn + "``|left|1366|\n\nadd_spacer|small|\nadd_textbox|`o" + pData->lastFrn + " is `4offline`o.``|\nadd_spacer|small|\nadd_button|removeconoff|`oRemove as friend``|0|0|\nadd_button|showoffline|`oBack``|0|0|\nadd_quick_exit|"));
							ENetPacket* packet2 = enet_packet_create(p2.data,
								p2.len,
								ENET_PACKET_FLAG_RELIABLE);
							enet_peer_send(peer, 0, packet2);
							delete p2.data;
						}
						if (btn == "addfriendrnbutton") {
							if (pData->haveGrowId == true) {
								try {
									for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
										if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
										if (static_cast<PlayerInfo*>(currentPeer->data)->rawName == pData->lastInfo) {
											if (pData->lastfriend == static_cast<PlayerInfo*>(currentPeer->data)->rawName) {
												pData->friendinfo.push_back(static_cast<PlayerInfo*>(currentPeer->data)->rawName); //add
												static_cast<PlayerInfo*>(currentPeer->data)->friendinfo.push_back(pData->rawName);
												string friends_string = "";
												for (int i = 0; i < pData->friendinfo.size(); i++) {
													friends_string += pData->friendinfo.at(i) + ",";
												}
												ifstream read_player("save/players/_" + pData->rawName + ".json");
												if (!read_player.is_open()) {
													continue;
												}
												json j;
												read_player >> j;
												read_player.close();
												j["friends"] = friends_string;
												ofstream write_player("save/players/_" + pData->rawName + ".json");
												write_player << j << std::endl;
												write_player.close();
												string friends_string2 = "";
												for (int i = 0; i < static_cast<PlayerInfo*>(currentPeer->data)->friendinfo.size(); i++) {
													friends_string2 += static_cast<PlayerInfo*>(currentPeer->data)->friendinfo.at(i) + ",";
												}
												ifstream read_player3("save/players/_" + static_cast<PlayerInfo*>(currentPeer->data)->rawName + ".json");
												if (!read_player3.is_open()) {
													continue;
												}
												json j3;
												read_player3 >> j3;
												read_player3.close();
												j3["friends"] = friends_string2;
												ofstream write_player3("save/players/_" + static_cast<PlayerInfo*>(currentPeer->data)->rawName + ".json");
												write_player3 << j3 << std::endl;
												write_player3.close();
												string text = "action|play_sfx\nfile|audio/love_in.wav\ndelayMS|0\n";
												BYTE* data = new BYTE[5 + text.length()];
												BYTE zero = 0;
												int type = 3;
												memcpy(data, &type, 4);
												memcpy(data + 4, text.c_str(), text.length());
												memcpy(data + 4 + text.length(), &zero, 1);
												ENetPacket* packet2 = enet_packet_create(data,
													5 + text.length(),
													ENET_PACKET_FLAG_RELIABLE);
												enet_peer_send(currentPeer, 0, packet2);
												enet_peer_send(peer, 0, packet2);
												delete[] data;
												GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnConsoleMessage"), "`3FRIEND ADDED: `oYou're now friends with `w" + pData->displayName + "`o!"));
												ENetPacket* packet = enet_packet_create(p.data,
													p.len,
													ENET_PACKET_FLAG_RELIABLE);
												enet_peer_send(currentPeer, 0, packet);
												delete p.data;
												GamePacket p3 = packetEnd(appendString(appendString(createPacket(), "OnConsoleMessage"), "`3FRIEND ADDED: `oYou're now friends with `w" + static_cast<PlayerInfo*>(currentPeer->data)->displayName + "`o!"));
												ENetPacket* packet3 = enet_packet_create(p3.data,
													p3.len,
													ENET_PACKET_FLAG_RELIABLE);
												enet_peer_send(peer, 0, packet3);
												delete p3.data;
											}
											else
											{
												GamePacket p4 = packetEnd(appendString(appendIntx(appendString(createPacket(), "OnTalkBubble"), pData->netID), "`5[`wFriend request sent to " + static_cast<PlayerInfo*>(currentPeer->data)->displayName + "`5]"));
												ENetPacket* packet4 = enet_packet_create(p4.data,
													p4.len,
													ENET_PACKET_FLAG_RELIABLE);
												enet_peer_send(peer, 0, packet4);
												delete p4.data;
												string text = "action|play_sfx\nfile|audio/tip_start.wav\ndelayMS|0\n";
												BYTE* data = new BYTE[5 + text.length()];
												BYTE zero = 0;
												int type = 3;
												memcpy(data, &type, 4);
												memcpy(data + 4, text.c_str(), text.length());
												memcpy(data + 4 + text.length(), &zero, 1);
												ENetPacket* packet2 = enet_packet_create(data,
													5 + text.length(),
													ENET_PACKET_FLAG_RELIABLE);
												enet_peer_send(currentPeer, 0, packet2);
												delete[] data;
												static_cast<PlayerInfo*>(currentPeer->data)->lastfriend = pData->rawName;
												GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnConsoleMessage"), "`3FRIEND REQUEST: `oYou've received a `wfriend request `ofrom `w" + pData->displayName + "`o! To accept, click the `wwrench by his/her name `oand then choose `wAdd as friend`o."));
												ENetPacket* packet = enet_packet_create(p.data,
													p.len,
													ENET_PACKET_FLAG_RELIABLE);
												enet_peer_send(currentPeer, 0, packet);
												delete p.data;
											}
										}
									}
								}
								catch (const std::out_of_range& e) {
									std::cout << e.what() << std::endl;
								}
								catch (std::exception& e) {
									std::cout << e.what() << std::endl;
								}
							}
							else
							{
								SendRegisterDialog(peer);
							}
						}
						break;
					}
					else if (cch.find("action|drop\n|itemID|") == 0)
					{
						if (pData->haveGrowId == true)
						{
							if (pData->isCursed == true)
							{
								continue;
							}
							try
							{
								if (pData->trade) end_trade(peer);
								std::stringstream ss(cch);
								std::string to;
								int idx = -1;
								int count = -1;
								while (std::getline(ss, to, '\n'))
								{
									vector<string> infoDat = explode("|", to);
									if (infoDat.size() == 3)
									{
										if (infoDat.at(1) == "itemID") idx = atoi(infoDat.at(2).c_str());
										if (infoDat.at(1) == "count") count = atoi(infoDat.at(2).c_str());
									}
								}
								pData->lastdropitem = idx;
								pData->lastdropitemcount = count;
								if (idx == -1) continue;
								if (itemDefs.size() < idx || idx < 0) continue;
								if (pData->lastdropitem == 6260 || getItemDef(pData->lastdropitem).properties & Property_Untradable || pData->lastdropitem == 18 || pData->lastdropitem == 32 || pData->lastdropitem == 6336 || pData->lastdropitem == 1794 || pData->lastdropitem == 5640 || pData->lastdropitem == 1424 || pData->lastdropitem == 6260)
								{
									if (!admin(peer) && !pData->Subscriber || pData->lastdropitem == 18 || pData->lastdropitem == 5640 || pData->lastdropitem == 32 || pData->lastdropitem == 6336 || pData->lastdropitem == 1486 || pData->lastdropitem == 1794 || pData->lastdropitem == 1424 || pData->lastdropitem == 6260)
									{
										Player::OnTextOverlay(peer, "You can't drop that.");
										continue;
									}
								}
								auto item = 0;
								for (auto i = 0; i < pData->inventory.items.size(); i++)
								{
									if (pData->inventory.items.at(i).itemID == pData->lastdropitem && pData->inventory.items.at(i).itemCount >= 1)
									{
										item = pData->inventory.items.at(i).itemCount;
										break;
									}
								}
								Player::OnDialogRequest(peer, "add_label_with_icon|big|`wDrop " + itemDefs.at(idx).name + "``|left|" + std::to_string(idx) + "|\nadd_textbox|`oHow many to drop?|\nadd_text_input|dropitemcount||" + to_string(item) + "|3|||\nadd_textbox|`4Warning:`` Any player who asks you to drop items is scamming you. We cannot restore scammed items.|left|\nend_dialog|dropdialog|Cancel|OK|\n");
							}
							catch (const std::out_of_range& e)
							{
								std::cout << e.what() << std::endl;
							}
						}
						break;
					}
					else if (cch.find("action|trash\n|itemID|") == 0)
					{
						try
						{
							if (pData->trade) end_trade(peer);
							std::stringstream ss(cch);
							std::string to;
							int idx = -1;
							int count = -1;
							while (std::getline(ss, to, '\n')) {
								vector<string> infoDat = explode("|", to);
								if (infoDat.size() == 3) {
									if (infoDat.at(1) == "itemID") idx = atoi(infoDat.at(2).c_str());
									if (infoDat.at(1) == "count") count = atoi(infoDat.at(2).c_str());
								}
							}
							pData->lasttrashitem = idx;
							if (idx == -1) continue;
							if (itemDefs.size() < idx || idx < 0) continue;
							if (pData->lasttrashitem == 18 || pData->lasttrashitem == 32 || pData->lasttrashitem == 6336) {
								if (getItemDef(pData->lasttrashitem).properties & Property_Untradable) {
								}
								else {
									Player::OnTextOverlay(peer, "You can't trash that.");
									continue;
								}
							}
							else {
								auto KiekTuri = 0;
								for (auto i = 0; i < pData->inventory.items.size(); i++) {
									if (pData->inventory.items.at(i).itemID == pData->lasttrashitem && pData->inventory.items.at(i).itemCount >= 1) {
										KiekTuri = pData->inventory.items.at(i).itemCount;
										break;
									}
								}
								if (getItemDef(idx).blockType == BlockTypes::FISH) {
									Player::OnDialogRequest(peer, "add_label_with_icon|big|`4Recycle `w" + itemDefs.at(idx).name + "``|left|" + std::to_string(idx) + "|\nadd_label|small|You will get up to 5 gems per item.|left|\nadd_textbox|`oHow many to `4destroy`o? (you have " + to_string(KiekTuri) + ")|\nadd_text_input|trashitemcount||0|3|\nend_dialog|trashdialog|Cancel|OK|\n");
								}
								else {
									Player::OnDialogRequest(peer, "add_label_with_icon|big|`4Trash `w" + itemDefs.at(idx).name + "``|left|" + std::to_string(idx) + "|\nadd_textbox|`oHow many to `4destroy`o? (you have " + to_string(KiekTuri) + ")|\nadd_text_input|trashitemcount||0|3|\nend_dialog|trashdialog|Cancel|OK|\n");
								}
							}
						}
						catch (const std::out_of_range& e) {
							std::cout << e.what() << std::endl;
						}
						break;
					}
					else if (cch.find("text|") != std::string::npos) {
						bool canchat = true;
						string str = cch.substr(cch.find("text|") + 5, cch.length() - cch.find("text|") - 1);
						if (str.length() > 120) continue;
						if (pData->currentWorld == "EXIT" || world == nullptr) continue;
						if (str.length() && str[0] == '/') {
							sendAction(peer, pData->netID, str);
							if (str.find("/msg ") != string::npos) {
								Player::OnConsoleMessage(peer, "CP:_PL:0_OID:_CT:[MSG]_ `6" + str);
							}
							else if (str.find("/r ") != string::npos) {
								Player::OnConsoleMessage(peer, "CP:_PL:0_OID:_CT:[MSG]_ `6" + str);
							}
							else if (str.find("/sb ") != string::npos) {
								Player::OnConsoleMessage(peer, "CP:_PL:0_OID:_CT:[SB]_ `6" + str);
							}
							else if (str.find("/ssb ") != string::npos) {
								Player::OnConsoleMessage(peer, "CP:_PL:0_OID:_CT:[CB]_ `6" + str);
							}
							else {
								Player::OnConsoleMessage(peer, "`6" + str);
							}
						}
						if (pData->taped == false) {
							if (world->silence == true && !isWorldOwner(peer, world)) {
								Player::OnTalkBubble(peer, pData->netID, "`1(Peasants must not to talk)", 0, true);
								continue;
							}
							SendChat(peer, pData->netID, str, world, cch);
						}
						else {
							for (char c : str) {
								if (str.find(" ")) canchat = false;
							}
							if (canchat) {
								if (pData->cantsay) {
									const string mf[4] = { "mf ff mf fm", "f fmf fmfmf fmm", "mfm ff mf mf", "mff ffmf mf " };
									string word = mf[rand() % 4];
									if (world->silence == true && !isWorldOwner(peer, world)) {
										Player::OnTalkBubble(peer, pData->netID, "`1(Peasants must not to talk)", 0, true);
										continue;
									}
									SendChat(peer, pData->netID, word, world, cch);
									continue;
								}
								else {
									pData->taped = false;
									pData->isDuctaped = false;
									pData->cantsay = false;
									send_state(peer);
									sendClothes(peer);
								}
							}
						}
						break;
					}
					else {
						cch.erase(std::remove(cch.begin(), cch.end(), '\n'), cch.end());
						if (find(unknown_packets.begin(), unknown_packets.end(), cch) == unknown_packets.end()) {
							unknown_packets.push_back(cch);
						}
						break;
					}
					break;
				}
				case 3:
				{
					stringstream ss(GetTextPointerFromPacket(event.packet));
					string cch = GetTextPointerFromPacket(event.packet);
					if (cout_cch) {
						cout << cch << endl;
					}
					if (cch.find("action|world_button") == 0) {
						if (cch.find("action|world_button\nname|_catselect_") == 0) {
							string worldOffers = "";
							worldOffers = "\nadd_button|Random|_0|0.8|3529161471|\nadd_button|Your Worlds|_16|0.8|3529161471|";
							auto p3 = packetEnd(appendString(appendString(createPacket(), "OnRequestWorldSelectMenu"), worldOffers));
							const auto packet3 = enet_packet_create(p3.data, p3.len, ENET_PACKET_FLAG_RELIABLE);
							enet_peer_send(peer, 0, packet3);
							delete p3.data;
						}
						else if (cch.find("action|world_button\nname|_0") == 0) {
							sendWorldOffers(peer);
						}
						else if (cch.find("action|world_button\nname|_16") == 0) {
							try {
								string worldOffers = "\nadd_button|Showing: `wYour Worlds``|_catselect_|0.6|3529161471|\n";
								std::sort(pData->worldsowned.begin(), pData->worldsowned.end());
								auto it = std::unique(pData->worldsowned.begin(), pData->worldsowned.end());
								pData->worldsowned.erase(it, pData->worldsowned.end());
								for (int i = 0; i < pData->worldsowned.size(); i++) {
									worldOffers += "add_floater|" + pData->worldsowned.at(i) + "|0|0.5|3417414143|\n";
								}
								auto p3 = packetEnd(appendString(appendString(createPacket(), "OnRequestWorldSelectMenu"), worldOffers));
								const auto packet3 = enet_packet_create(p3.data, p3.len, ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet3);
								delete p3.data;
							}
							catch (const std::out_of_range& e) {
								std::cout << e.what() << std::endl;
							}
						}
						break;
					}
					string to = "", name = "";
					bool isJoinReq = false;
					while (std::getline(ss, to, '\n')) {
						if (to.find('|') == -1) continue;
						string id = "";
						id = to.substr(0, to.find("|"));
						string act = "";
						act = to.substr(to.find("|") + 1, to.length() - to.find("|") - 1);
						if (id == "invitedWorld") {
							handle_world(peer, name);
						}
						if (id == "name" && isJoinReq) {
							name = act;
						}
						if (id == "action") {
							if (act == "join_request") {
								isJoinReq = true;
							}
							if (act == "quit_to_exit") {
								if (pData->HasLogged || !pData->haveGrowId) {
									if (pData->currentWorld == "EXIT") {
										enet_peer_disconnect_later(peer, 0);
										break;
									}
									Player::OnConsoleMessage(peer, "Where would you like to go? (`w" + GetPlayerCountServer() + " `oonline)");
									sendPlayerLeave(peer);
									sendWorldOffers(peer);
								}
							}
							if (act == "quit") {
								enet_peer_disconnect_later(peer, 0);
							}
						}
					}
					break;
				}
				case 4:
				{
					if (pData->currentWorld == "EXIT" || world == NULL) continue;
					BYTE* tankUpdatePacket = GetStructPointerFromTankPacket(event.packet);
					if (tankUpdatePacket) {
						PlayerMoving* pMov = unpackPlayerMoving(tankUpdatePacket);
						if (!pData->canWalkInBlocks) {
							if ((pMov->characterState >= 80 || pMov->characterState == 64) && pMov->characterState != 160 && pMov->characterState != 144 && pMov->characterState != 128 && pMov->characterState < 250) {
								pData->lavaLevel = pData->lavaLevel + 1;
								if (pData->lavaLevel >= 5) {
									pData->lavaLevel = 0;
									playerRespawn(world, peer, false);
								}
							}
						}
						if (world->name == "LEGENDARY") {
							try
							{
								for (auto i = 0; i < world->width * world->height; i++)
								{
									if (world->items.at(i).foreground == 1792)
									{
										int x = i % world->width, y = i / world->width;
										if (pData->x / 32 == x && pData->y / 32 == y)
										{
											bool iscontains = false;
											SearchInventoryItem(peer, 1794, 1, iscontains);
											if (!iscontains) {
												bool success = false;
												SaveItemMoreTimes(1794, 1, peer, success);
												for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer)
												{
													if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
													if (isHere(peer, currentPeer))
													{
														Player::OnParticleEffect(currentPeer, 46, pData->x, pData->y, 0);
													}
												}
												Player::OnTalkBubble(peer, pData->netID, "`9You have claimed a Legendary Orb!", 0, true);
											}
										}
										break;
									}
								}
							}
							catch (const std::out_of_range& e) {
								std::cout << e.what() << std::endl;
							}
						}
						switch (pMov->packetType) {
						case 0:
						{
							pData->x = pMov->x;
							pData->y = pMov->y;
							pData->isRotatedLeft = pMov->characterState & 0x10;
							sendPData(peer, pMov);
							if (pData->isinv == false) {
								if (pData->cloth_necklace == 9168) {
									if (!visibleparticles) {
										Player::OnParticleEffect(peer, 125, pData->x, pData->y, 0);
									}
									else {
										for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
											if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
											if (isHere(peer, currentPeer)) {
												Player::OnParticleEffect(currentPeer, 125, pData->x, pData->y, 0);
											}
										}
									}
								}
								if (pData->cloth_back == 9152) {
									if (!visibleparticles) {
										Player::OnParticleEffect(peer, 125, pData->x, pData->y, 0);
									}
									else {
										for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
											if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
											if (isHere(peer, currentPeer)) {
												Player::OnParticleEffect(currentPeer, 359, pData->x, pData->y, 0);
											}
										}
									}
								}
								else if (pData->cloth_back == 9222) {
									if (!visibleparticles) {
										Player::OnParticleEffect(peer, 125, pData->x, pData->y, 0);
									}
									else {
										for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
											if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
											if (isHere(peer, currentPeer)) {
												Player::OnParticleEffect(currentPeer, 321, pData->x + rand() % 35 - 20, pData->y + rand() % 35 - 20, 0);
												Player::OnParticleEffect(currentPeer, 322, pData->x + rand() % 35 - 20, pData->y + rand() % 35 - 20, 0);
												Player::OnParticleEffect(currentPeer, 321, pData->x + rand() % 35 - 20, pData->y + rand() % 35 - 20, 0);
												Player::OnParticleEffect(currentPeer, 322, pData->x + rand() % 35 - 20, pData->y + rand() % 35 - 20, 0);
												Player::OnParticleEffect(currentPeer, 321, pData->x + rand() % 35 - 20, pData->y + rand() % 35 - 20, 0);
												Player::OnParticleEffect(currentPeer, 322, pData->x + rand() % 35 - 20, pData->y + rand() % 35 - 20, 0);
											}
										}
									}
								}
								else if (pData->cloth_back == 9154) {
									if (!visibleparticles) {
										Player::OnParticleEffect(peer, 125, pData->x, pData->y, 0);
									}
									else {
										for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
											if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
											if (isHere(peer, currentPeer)) {
												Player::OnParticleEffect(currentPeer, 150, pData->x, pData->y, 0);
											}
										}
									}
								}
								else if (pData->cloth_back == 9162) {
									if (!visibleparticles) {
										Player::OnParticleEffect(peer, 125, pData->x, pData->y, 0);
									}
									else {
										for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
											if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
											if (isHere(peer, currentPeer)) {
												Player::OnParticleEffect(currentPeer, 215, pData->x, pData->y, 0);
											}
										}
									}
								}
								if (pData->cloth_hand == 2204) {
									if (pData->lastSYNC + 400 < (duration_cast<milliseconds>(system_clock::now().time_since_epoch())).count())
									{
										pData->lastSYNC = (duration_cast<milliseconds>(system_clock::now().time_since_epoch())).count();
										find_geiger(peer);
									}
								}
							}
							if (!pData->joinClothesUpdated) {
								pData->joinClothesUpdated = true;
								updateAllClothes(peer);
								updateInvis(peer);
								send_state(peer);
							}
							break;
						}
						case 11:
						{
							if (world == nullptr || pData->currentWorld == "EXIT" || serverIsFrozen) break;
							if (!world) break;
							if (world->DisableDrop && pData->rawName != world->owner && !admin(peer)) {
								Player::OnTextOverlay(peer, "Collecting is disabled here!");
								break;
							}
							try {
								int blockX = static_cast<int>(pMov->x) / 32;
								int blockY = static_cast<int>(pMov->y) / 32;
								if (blockX < 0 || blockY < 0 || blockX > world->width || blockY > world->height) break;
								if (world->items.at(blockX + (blockY * world->width)).foreground != 0 && getItemDef(world->items.at(blockX + (blockY * world->width)).foreground).blockType == BlockTypes::FOREGROUND || world->items.at(blockX + (blockY * world->width)).foreground != 0 && getItemDef(world->items.at(blockX + (blockY * world->width)).foreground).blockType == BlockTypes::DISPLAY) {
									break;
								}
								bool legit = true;
								int itemIdx = pMov->plantingTree - 1;
								int atik = -1;
								try
								{
									for (int i = 0; i < world->droppedItems.size(); i++)
									{
										if (world->droppedItems.at(i).uid == itemIdx)
										{
											atik = i;
											break;
										}
									}
								}
								catch (...)
								{
									break;
								}
								legit = atik != -1;
								if (legit)
								{
									DroppedItem droppedItem = world->droppedItems.at(atik);
									legit = true;
								}
								if (legit) {
									if (pData->x / 32 == blockX || pData->x / 32 + 1 == blockX || pData->x / 32 - 1 == blockX || pData->x / 32 + 2 == blockX || pData->x / 32 - 2 == blockX) {
										DroppedItem droppedItem = world->droppedItems.at(atik);
										if (getItemDef(droppedItem.id).blockType == BlockTypes::FISH) {
											auto iscontains = false;
											SearchInventoryItem(peer, droppedItem.id, 1, iscontains);
											if (iscontains) {
												Player::OnTalkBubble(peer, pData->netID, "`1(Item maxed)", 0, true);
												break;
											}
										} if (droppedItem.id != 112) {
											if (CheckItemMaxed(peer, droppedItem.id, droppedItem.count)) {
												if (atoi(GetItemCount(peer, droppedItem.id).c_str()) < 250 && droppedItem.count > (250 - atoi(GetItemCount(peer, droppedItem.id).c_str()))) {
													int xdrop = droppedItem.x;
													int ydrop = droppedItem.y;
													int iddrop = droppedItem.id;
													int give_count = (250 - atoi(GetItemCount(peer, droppedItem.id).c_str()));
													int idcount = droppedItem.count - (250 - atoi(GetItemCount(peer, droppedItem.id).c_str()));
													world->droppedItems.erase(world->droppedItems.begin() + atik);
													SendTake(peer, pData->netID, pMov->x, pMov->y, itemIdx + 1);
													DropItem(world, peer, -1, xdrop, ydrop, iddrop, idcount, 0);
													if (getItemDef(droppedItem.id).rarity == 999) {
														Player::OnConsoleMessage(peer, "`oCollected `w" + to_string(give_count) + " " + getItemDef(iddrop).name + "`o.");
													}
													else {
														Player::OnConsoleMessage(peer, "`oCollected `w" + to_string(give_count) + " " + getItemDef(iddrop).name + "`o. Rarity: `w" + to_string(getItemDef(droppedItem.id).rarity) + "`o.");
													}
													auto success = true;
													SaveDroppedItem(iddrop, give_count, peer, success);
												}
												else {
													Player::OnTalkBubble(peer, pData->netID, "`1(Item maxed)", 0, true);
												}
												break;
											} if (CheckItemExists(peer, droppedItem.id) == false) {
												if (pData->inventory.items.size() == pData->currentInventorySize) {
													Player::OnTalkBubble(peer, pData->netID, "`1(Inventory is full)", 0, true);
													break;
												}
											}
										} if (droppedItem.id == 112) {
											if (droppedItem.count == 1) world->items.at(blockX + (blockY * world->width)).yellowGems--;
											if (droppedItem.count == 5) world->items.at(blockX + (blockY * world->width)).blueGems--;
											if (droppedItem.count == 10) world->items.at(blockX + (blockY * world->width)).redGems--;
											if (droppedItem.count == 50) world->items.at(blockX + (blockY * world->width)).greenGems--;
											if (droppedItem.count == 100) world->items.at(blockX + (blockY * world->width)).purpleGems--;
											try
											{
												std::ifstream ifsz("save/gemdb/_" + pData->rawName + ".txt");
												if (!ifsz.is_open()) {
													ColoredText("TXT Exception #2 Failed to open TXT for reading " + pData->rawName, "red");
													break;
												}
												std::string content((std::istreambuf_iterator<char>(ifsz)), (std::istreambuf_iterator<char>()));
												int gembux = atoi(content.c_str());
												int fingembux = gembux + droppedItem.count;
												ofstream myfile;
												myfile.open("save/gemdb/_" + pData->rawName + ".txt");
												myfile << fingembux;
												myfile.close();
												int gemcalc = gembux + droppedItem.count;
												Player::OnSetBux(peer, gemcalc, 0);
												const auto p3 = packetEnd(appendFloat(appendIntx(appendFloat(appendFloat(appendFloat(appendString(createPacket(), "OnSetClothing"), (static_cast<PlayerInfo*>(peer->data))->cloth_hair, (static_cast<PlayerInfo*>(peer->data))->cloth_shirt, (static_cast<PlayerInfo*>(peer->data))->cloth_pants), (static_cast<PlayerInfo*>(peer->data))->cloth_feet, (static_cast<PlayerInfo*>(peer->data))->cloth_face, (static_cast<PlayerInfo*>(peer->data))->cloth_hand), (static_cast<PlayerInfo*>(peer->data))->cloth_back, (static_cast<PlayerInfo*>(peer->data))->cloth_mask, (static_cast<PlayerInfo*>(peer->data))->cloth_necklace), (static_cast<PlayerInfo*>(peer->data))->skinColor), (static_cast<PlayerInfo*>(peer->data))->cloth_ances, 0.0f, 0.0f));
												memcpy(p3.data + 8, &((static_cast<PlayerInfo*>(peer->data))->netID), 4);
												const auto packet = enet_packet_create(p3.data, p3.len, ENET_PACKET_FLAG_RELIABLE);
												enet_peer_send(peer, 0, packet);
												delete p3.data;
											}
											catch (std::exception& e)
											{
												std::cout << e.what() << std::endl;
												break;
											}
										}
										else
										{
											auto success = true;
											SaveDroppedItem(droppedItem.id, droppedItem.count, peer, success);
										}
										if (getItemDef(droppedItem.id).rarity == 999) {
											if (droppedItem.id != 112) {
												Player::OnConsoleMessage(peer, "`oCollected `w" + to_string(droppedItem.count) + " " + getItemDef(droppedItem.id).name + "`o.");
											}
										}
										else
										{
											Player::OnConsoleMessage(peer, "`oCollected `w" + to_string(droppedItem.count) + " " + getItemDef(droppedItem.id).name + "`o. Rarity: `w" + to_string(getItemDef(droppedItem.id).rarity) + "`o.");
										}
										world->droppedItems.erase(world->droppedItems.begin() + atik);
										SendTake(peer, pData->netID, pMov->x, pMov->y, itemIdx + 1);
									}
								}
							}
							catch (const std::out_of_range& e) {
								std::cout << e.what() << std::endl;
							}
							break;
						}
						case 23:
						{
							try {
								int hit_x = (pMov->plantingTree % world->width);
								int hit_y = (pMov->plantingTree / world->width);
								for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
									if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
									if (isHere(peer, currentPeer)) {
										if (pData->x / 32 == hit_x && pData->y / 32 == hit_y || pData->x / 32 + 1 == hit_x && pData->y / 32 == hit_y || pData->x / 32 + 2 == hit_x && pData->y / 32 == hit_y || pData->x / 32 - 1 == hit_x && pData->y / 32 == hit_y || pData->x / 32 - 2 == hit_x && pData->y / 32 == hit_y) {
											if (static_cast<PlayerInfo*>(currentPeer->data)->rawName == pData->rawName) continue;
											if (jammers) {
												bool punchJammer = false;
												for (auto i = 0; i < world->width * world->height; i++) {
													if (world->items.at(i).foreground == 1276 && world->items.at(i).activated) {
														punchJammer = true;
														break;
													}
												}
												if (punchJammer) break;
											}
											if (static_cast<PlayerInfo*>(currentPeer->data)->cloth_hand == 3466) {
												static_cast<PlayerInfo*>(currentPeer->data)->TotalKills++;
												if (static_cast<PlayerInfo*>(currentPeer->data)->quest_active && static_cast<PlayerInfo*>(currentPeer->data)->lastquest == "honor" && static_cast<PlayerInfo*>(currentPeer->data)->quest_step == 2 && static_cast<PlayerInfo*>(currentPeer->data)->quest_progress < 100) {
													static_cast<PlayerInfo*>(currentPeer->data)->quest_progress++;
													if (static_cast<PlayerInfo*>(currentPeer->data)->quest_progress >= 100) {
														static_cast<PlayerInfo*>(currentPeer->data)->quest_progress = 100;
														Player::OnTalkBubble(currentPeer, static_cast<PlayerInfo*>(currentPeer->data)->netID, "`9Legendary Quest step complete! I'm off to see a Wizard!", 0, false);
													}
												}
												playerRespawn(world, peer, false);
											}
											else if (static_cast<PlayerInfo*>(currentPeer->data)->isZombie == true && pData->isZombie == false)
											{
												if (static_cast<PlayerInfo*>(currentPeer->data)->rawName == pData->rawName) continue;
												if (jammers)
												{
													bool zombieJammer = false;
													for (auto i = 0; i < world->width * world->height; i++)
													{
														if (world->items.at(i).foreground == 1278 && world->items.at(i).activated)
														{
															zombieJammer = true;
															break;
														}
													}
													if (zombieJammer) break;
												}
												if (pData->canWalkInBlocks == true) {
													pData->canWalkInBlocks = false;
													pData->skinColor = 0x8295C3FF;
													send_state(peer);
												}
												Player::OnAddNotification(peer, "`4You were infected by " + static_cast<PlayerInfo*>(currentPeer->data)->rawName + "`4!", "audio/skel.wav", "interface/large/infected.rttex");
												pData->isZombie = true;
												playerconfig(peer, 1150, 130, 0x14);
											}
											if (jammers)
											{
												bool minimod = false;
												for (auto i = 0; i < world->width * world->height; i++)
												{
													if (world->items.at(i).foreground == 4758 && world->items.at(i).activated)
													{
														minimod = true;
														break;
													}
													if (minimod)
													{
														world->DisableDrop = true;
													}
												}
											}
										}
									}
								}
							}
							catch (const std::out_of_range& e) {
								std::cout << e.what() << std::endl;
							}
							break;
						}
						case 7:
						{
							if (pData->currentWorld == "EXIT") continue;
							try {
								if (pMov->punchX < 0 || pMov->punchY < 0 || pMov->punchX > 100 || pMov->punchY > 100) continue;
								int idx = pMov->punchY * world->width + pMov->punchX;
								if (getItemDef(world->items.at(idx).foreground).blockType == BlockTypes::DOOR || getItemDef(world->items.at(idx).foreground).blockType == BlockTypes::PORTAL) {
									if (world->items.at(idx).foreground == 762) {
										auto respawnTimeout = 200;
										auto deathFlag = 0x19;
										auto p23 = packetEnd(appendIntx(appendFloat(appendString(createPacket(), "OnZoomCamera"), 10000.000000), 1000));
										memcpy(p23.data + 24, &respawnTimeout, 4);
										memcpy(p23.data + 56, &deathFlag, 4);
										const auto packet2 = enet_packet_create(p23.data, p23.len, ENET_PACKET_FLAG_RELIABLE);
										enet_peer_send(peer, 0, packet2);
										delete p23.data;
										auto p234 = packetEnd(appendIntx(appendString(createPacket(), "OnSetFreezeState"), 0));
										memcpy(p234.data + 8, &(pData->netID), 4);
										memcpy(p234.data + 24, &respawnTimeout, 4);
										memcpy(p234.data + 56, &deathFlag, 4);
										const auto packet23 = enet_packet_create(p234.data, p234.len, ENET_PACKET_FLAG_RELIABLE);
										enet_peer_send(peer, 0, packet23);
										delete p234.data;
										string DoorLabelDialog = getItemDef(world->items.at(idx).foreground).name;
										if (world->items.at(idx).label != "") DoorLabelDialog = world->items.at(idx).label;
										pData->lastPunchX = pMov->punchX;
										pData->lastPunchY = pMov->punchY;
										pData->wrenchsession = pMov->punchX + (pMov->punchY * world->width);
										auto p234s = packetEnd(appendString(appendString(createPacket(), "OnDialogRequest"), "set_default_color|`o\nadd_label_with_icon|big|`w" + DoorLabelDialog + "``|left|762|\nadd_textbox|The door requires a password.|left|\nadd_text_input|password|Password||24|\nend_dialog|password_reply|Cancel|OK|"));
										memcpy(p234s.data + 24, &respawnTimeout, 4);
										memcpy(p234s.data + 56, &deathFlag, 4);
										const auto packet234 = enet_packet_create(p234s.data, p234s.len, ENET_PACKET_FLAG_RELIABLE);
										enet_peer_send(peer, 0, packet234);
										delete p234s.data;
									}
									else {
										DoEnterDoor(peer, world, pMov->punchX, pMov->punchY);
									}
								}
								else if (getItemDef(world->items.at(idx).foreground).blockType == BlockTypes::MAIN_DOOR) {
									Player::OnConsoleMessage(peer, "Where would you like to go? (`w" + GetPlayerCountServer() + " `oonline)");
									sendPlayerLeave(peer);
									sendWorldOffers(peer);
								}
							}
							catch (const std::out_of_range& e) {
								std::cout << e.what() << std::endl;
							}
							break;
						}
						case 10:
						{
							if (pData->currentWorld == "EXIT") continue;
							try {
								if (pData->trade) end_trade(peer);
								if (pMov->plantingTree == 6336) {
									SendGrowpedia(peer);
									break;
								}
								if (pMov->plantingTree == 242)
								{
									bool iscontainseas = false;
									SearchInventoryItem(peer, pMov->plantingTree, 1, iscontainseas);
									if (!iscontainseas) break;
									for (int i = 0; i < pData->inventory.items.size(); i++)
									{
										if (pData->inventory.items.at(i).itemID == 242)
										{
											if (pData->inventory.items.at(i).itemCount >= 100)
											{
												bool isValid = SaveConvertedItem(1796, 1, peer);
												if (isValid)
												{
													RemoveInventoryItem(pMov->plantingTree, 100, peer, true);
													Player::OnTalkBubble(peer, pData->netID, "`wYou compressed 100 `2World Lock `winto a `2Diamond Lock`w!", 0, true);
													Player::OnConsoleMessage(peer, "`oYou compressed 100 `2World Lock `ointo a `2Diamond Lock`o!");
												}
											}
										}
									}
									break;
								}
								else if (pMov->plantingTree == 1796)
								{
									bool iscontainseas = false;
									SearchInventoryItem(peer, pMov->plantingTree, 1, iscontainseas);
									if (!iscontainseas) break;
									for (int i = 0; i < pData->inventory.items.size(); i++)
									{
										if (pData->inventory.items.at(i).itemID == 1796)
										{
											if (pData->inventory.items.at(i).itemCount >= 1)
											{
												bool isValid = SaveConvertedItemDls(242, 100, peer);
												if (isValid)
												{
													RemoveInventoryItem(pMov->plantingTree, 1, peer, true);
													Player::OnTalkBubble(peer, pData->netID, "`wYou shattered a `2Diamond Lock `winto 100 `2World Lock`w!", 0, true);
													Player::OnConsoleMessage(peer, "`oYou shattered a `2Diamond Lock `ointo 100 `2World Lock`o!");
												}
											}
										}
									}
									break;
								}
								else if (pMov->plantingTree == 7188)
								{
									bool iscontainseas = false;
									SearchInventoryItem(peer, pMov->plantingTree, 1, iscontainseas);
									if (!iscontainseas) break;
									for (int i = 0; i < pData->inventory.items.size(); i++)
									{
										if (pData->inventory.items.at(i).itemID == 7188)
										{
											if (pData->inventory.items.at(i).itemCount >= 1)
											{
												bool isValid = SaveConvertedItemDls(1796, 100, peer);
												if (isValid)
												{
													RemoveInventoryItem(pMov->plantingTree, 1, peer, true);
													Player::OnTalkBubble(peer, pData->netID, "`wYou shattered a `2Blue Gem Lock `winto 100 `2Diamond Lock`w!", 0, true);
													Player::OnConsoleMessage(peer, "`oYou shattered a `2Blue Gem Lock `ointo 100 `2Diamond Lock`o!");
												}
											}
										}
									}
									break;
								}
								if (pData->Fishing) {
									Player::OnTalkBubble(peer, pData->netID, "`wYou cant do that while fishing`w!", 0, true);
									continue;
								}
								equip_item(peer, pMov->plantingTree, getItemDef(pMov->plantingTree).clothType);
							}
							catch (const std::out_of_range& e) {
								std::cout << e.what() << std::endl;
							}
							break;
						}
						case 18:
						{
							sendPData(peer, pMov);
							break;
						}
						default:
						{
							break;
						}
						}
						if (pMov->punchX != -1 && pMov->punchY != -1) {
							if (pMov->packetType == 3) {
								if (pData->currentWorld == "EXIT") continue;
								if (pData->isRotatedLeft = pMov->characterState & 0x10) {
									pData->RotatedLeft = true;
								}
								else {
									pData->RotatedLeft = false;
								}
								if (pData->trade) end_trade(peer);
								if (pData->lastBREAK + 150 < (duration_cast<milliseconds>(system_clock::now().time_since_epoch())).count()) {
									pData->lastBREAK = (duration_cast<milliseconds>(system_clock::now().time_since_epoch())).count();
									if (pMov->plantingTree == 18) {
										if (pData->cloth_hand == 5480 || triple_punch.find(pData->cloth_hand) != triple_punch.end() || triple_punch.find(pData->cloth_back) != triple_punch.end() || triple_punch.find(pData->cloth_necklace) != triple_punch.end()) {
											if (pData->cloth_face != 4288 && pData->isZombie == false) {
												if (pData->isRotatedLeft = pMov->characterState & 0x10) {
													pData->RotatedLeft = true;
												}
												else {
													pData->RotatedLeft = false;
												} if (pMov->punchY == pData->y / 32) {
													if (pData->RotatedLeft == true) {
														sendTileUpdate(pMov->punchX, pMov->punchY, pMov->plantingTree, pData->netID, peer, world);
														sendTileUpdate(pMov->punchX - 1, pMov->punchY, pMov->plantingTree, pData->netID, peer, world);
														sendTileUpdate(pMov->punchX - 2, pMov->punchY, pMov->plantingTree, pData->netID, peer, world);
													}
													else {
														sendTileUpdate(pMov->punchX, pMov->punchY, pMov->plantingTree, pData->netID, peer, world);
														sendTileUpdate(pMov->punchX + 1, pMov->punchY, pMov->plantingTree, pData->netID, peer, world);
														sendTileUpdate(pMov->punchX + 2, pMov->punchY, pMov->plantingTree, pData->netID, peer, world);
													}
												}
												else if (pMov->punchX == pData->x / 32) {
													if (pMov->punchY > pData->y / 32) {
														sendTileUpdate(pMov->punchX, pMov->punchY, pMov->plantingTree, pData->netID, peer, world);
														sendTileUpdate(pMov->punchX, pMov->punchY + 1, pMov->plantingTree, pData->netID, peer, world);
														sendTileUpdate(pMov->punchX, pMov->punchY + 2, pMov->plantingTree, pData->netID, peer, world);
													}
													else if (pMov->punchY < pData->y / 32) {
														sendTileUpdate(pMov->punchX, pMov->punchY, pMov->plantingTree, pData->netID, peer, world);
														sendTileUpdate(pMov->punchX, pMov->punchY - 1, pMov->plantingTree, pData->netID, peer, world);
														sendTileUpdate(pMov->punchX, pMov->punchY - 2, pMov->plantingTree, pData->netID, peer, world);
													}
													else {
														sendTileUpdate(pMov->punchX, pMov->punchY, pMov->plantingTree, pData->netID, peer, world);
													}
												}
												else if (pMov->punchY < pData->y / 32) {
													if (pData->RotatedLeft == true) {
														sendTileUpdate(pMov->punchX, pMov->punchY, pMov->plantingTree, pData->netID, peer, world);
														sendTileUpdate(pMov->punchX - 1, pMov->punchY - 1, pMov->plantingTree, pData->netID, peer, world);
														sendTileUpdate(pMov->punchX - 2, pMov->punchY - 2, pMov->plantingTree, pData->netID, peer, world);
													}
													else {
														sendTileUpdate(pMov->punchX, pMov->punchY, pMov->plantingTree, pData->netID, peer, world);
														sendTileUpdate(pMov->punchX + 1, pMov->punchY - 1, pMov->plantingTree, pData->netID, peer, world);
														sendTileUpdate(pMov->punchX + 2, pMov->punchY - 2, pMov->plantingTree, pData->netID, peer, world);
													}
												}
												else if (pMov->punchY < pData->y / 32) {
													if (pData->RotatedLeft == true) {
														sendTileUpdate(pMov->punchX, pMov->punchY, pMov->plantingTree, pData->netID, peer, world);
														sendTileUpdate(pMov->punchX - 1, pMov->punchY - 1, pMov->plantingTree, pData->netID, peer, world);
														sendTileUpdate(pMov->punchX - 2, pMov->punchY - 2, pMov->plantingTree, pData->netID, peer, world);
													}
													else {
														sendTileUpdate(pMov->punchX, pMov->punchY, pMov->plantingTree, pData->netID, peer, world);
														sendTileUpdate(pMov->punchX + 1, pMov->punchY - 1, pMov->plantingTree, pData->netID, peer, world);
														sendTileUpdate(pMov->punchX + 2, pMov->punchY - 2, pMov->plantingTree, pData->netID, peer, world);
													}
												}
												else if (pMov->punchY > pData->y / 32) {
													if (pData->RotatedLeft == true) {
														sendTileUpdate(pMov->punchX, pMov->punchY, pMov->plantingTree, pData->netID, peer, world);
														sendTileUpdate(pMov->punchX - 1, pMov->punchY + 1, pMov->plantingTree, pData->netID, peer, world);
														sendTileUpdate(pMov->punchX - 2, pMov->punchY + 2, pMov->plantingTree, pData->netID, peer, world);
													}
													else {
														sendTileUpdate(pMov->punchX, pMov->punchY, pMov->plantingTree, pData->netID, peer, world);
														sendTileUpdate(pMov->punchX + 1, pMov->punchY + 1, pMov->plantingTree, pData->netID, peer, world);
														sendTileUpdate(pMov->punchX + 2, pMov->punchY + 2, pMov->plantingTree, pData->netID, peer, world);
													}
												}
											}
											else {
												sendTileUpdate(pMov->punchX, pMov->punchY, pMov->plantingTree, pData->netID, peer, world);
											}
										}
										else {
											sendTileUpdate(pMov->punchX, pMov->punchY, pMov->plantingTree, pData->netID, peer, world);
										}
									}
									else {
										if (triple_place.find(pData->cloth_hand) != triple_place.end() || triple_place.find(pData->cloth_hair) != triple_place.end() || triple_place.find(pData->cloth_necklace) != triple_place.end() || triple_place.find(pData->cloth_back) != triple_place.end()) {
											if ((static_cast<PlayerInfo*>(peer->data))->isRotatedLeft = pMov->characterState & 0x10) {
												(static_cast<PlayerInfo*>(peer->data))->RotatedLeft = true;
											}
											else {
												(static_cast<PlayerInfo*>(peer->data))->RotatedLeft = false;
											} if ((static_cast<PlayerInfo*>(peer->data))->RotatedLeft == true) {
												sendTileUpdate(pMov->punchX, pMov->punchY, pMov->plantingTree, (static_cast<PlayerInfo*>(peer->data))->netID, peer, world);
												sendTileUpdate(pMov->punchX - 1, pMov->punchY, pMov->plantingTree, (static_cast<PlayerInfo*>(peer->data))->netID, peer, world);
												sendTileUpdate(pMov->punchX - 2, pMov->punchY, pMov->plantingTree, (static_cast<PlayerInfo*>(peer->data))->netID, peer, world);
											}
											else {
												sendTileUpdate(pMov->punchX, pMov->punchY, pMov->plantingTree, (static_cast<PlayerInfo*>(peer->data))->netID, peer, world);
												sendTileUpdate(pMov->punchX + 1, pMov->punchY, pMov->plantingTree, (static_cast<PlayerInfo*>(peer->data))->netID, peer, world);
												sendTileUpdate(pMov->punchX + 2, pMov->punchY, pMov->plantingTree, (static_cast<PlayerInfo*>(peer->data))->netID, peer, world);
											}
										}
										else {
											sendTileUpdate(pMov->punchX, pMov->punchY, pMov->plantingTree, (static_cast<PlayerInfo*>(peer->data))->netID, peer, world);
										}
									}
								}
							}
						}
						delete pMov;
					}
				}
				default:
				{
					break;
				}
				}
				enet_packet_destroy(event.packet);
				break;
			}
			case ENET_EVENT_TYPE_DISCONNECT:
			{
				ENetPeer* peer = event.peer;
				if (peer->data != NULL)
				{
					PlayerInfo* pData = GetPeerData(peer);
					if (pData->haveGrowId && pData->isIn && pData->HasLogged && pData->rawName != "")
					{
						for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
							if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
							string name = static_cast<PlayerInfo*>(currentPeer->data)->rawName;
							if (find(pData->friendinfo.begin(), pData->friendinfo.end(), name) != pData->friendinfo.end()) {
								Player::PlayAudio(currentPeer, "audio/friend_logoff.wav", 0);
								Player::OnConsoleMessage(currentPeer, "`!FRIEND ALERT : `w" + pData->displayName + " `ohas `4logged off`o.");
							}
						}
						save_playerinfo(pData);
					}
					sendPlayerLeave(peer);
				}
				break;
			}
			default:
			{
				break;
			}
			}
		}
	}
	system("PAUSE");
};
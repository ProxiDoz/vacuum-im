#ifndef DEF_ROSTERINDEXROLES_H
#define DEF_ROSTERINDEXROLES_H

enum RosterIndexDataRoles {
	RDR_ANY_ROLE = 32,
	RDR_KIND,
	RDR_KIND_ORDER,
	RDR_SORT_ORDER,
	//XMPP Roles
	RDR_STREAMS,
	RDR_STREAM_JID,
	RDR_FULL_JID,
	RDR_PREP_FULL_JID,
	RDR_PREP_BARE_JID,
	RDR_RESOURCES,
	RDR_NAME,
	RDR_GROUP,
	RDR_SHOW,
	RDR_STATUS,
	RDR_PRIORITY,
	RDR_SUBSCRIBTION,
	RDR_ASK,
	//View roles
	RDR_LABEL_ITEMS,
	RDR_FORCE_VISIBLE,
	RDR_STATES_FORCE_ON,
	RDR_STATES_FORCE_OFF,
	//Avatars
	RDR_AVATAR_HASH,
	RDR_AVATAR_IMAGE,
	//Annotations
	RDR_ANNOTATIONS,
	//Recent Items
	RDR_RECENT_TYPE,
	RDR_RECENT_REFERENCE,
	RDR_RECENT_DATETIME,
	//MultiUser Chats
	RDR_MUC_NICK,
	RDR_MUC_PASSWORD,
	//vCard
	RDR_VCARD_SEARCH,
	//MetaContacts
	RDR_METACONTACT_ID,
	//Other Roles
	RDR_USER_ROLE = 128
};

#endif //DEF_ROSTERINDEXROLES_H

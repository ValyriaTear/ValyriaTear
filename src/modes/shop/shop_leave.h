///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    shop_leave.h
*** \author  Tyler Olsen, roots@allacrost.org
*** \brief   Header file for leave interface of shop mode
***
*** \note The contents of this file are near identical to the contents of
*** shop_confirm.h. When making any changes to this file, please look to
*** shop_confirm.h to see if it should have similar changes made.
*** ***************************************************************************/

#ifndef __SHOP_LEAVE_HEADER__
#define __SHOP_LEAVE_HEADER__

#include "defs.h"
#include "utils.h"

#include "video.h"
#include "global.h"

#include "shop_utils.h"

namespace hoa_shop {

namespace private_shop {

/** ****************************************************************************
*** \brief An interface to prevent the player from leaving the shop with marked transactions
***
*** This interface is only entered if the player tries to leave the shop and they still
*** have marked purchases, sales, or trades. Its primary purpose is to serve as a fail-safe
*** so that the player doesn't accidentally leave the shop before they complete their transaction.
*** The class itself is similar in display and function to the ConfirmInterface, but it uses a
*** reduced subset of the abilities found in that interface. Specifically the player may not modify
*** their order in any way from this interface, although they can view their order and selected
*** objects in that order.
***
***
*** After the player has selected their purchases, sales, and trades, they typically
*** enter this interface to finalize their choices and complete the transaction. There
*** are four primary actions that a player may chose from while in this interface.
***
*** -# "View Order": allows the player to view the buy/sell/trade lists and selected entries
*** -# "Confirm Order": sends the player to the confirm interface so that the order can be completed
*** -# "Leave Shop": leaves shop mode entirely without further interruption
*** ***************************************************************************/
class LeaveInterface : public ShopInterface {
	//! \brief States of the leave interface used to determine how to process user input and what information to draw
	enum LEAVE_STATE {
		LEAVE_STATE_INVALID       = -1,
		LEAVE_STATE_MAIN          =  0, //!< User input is focused on the main interface prompt
		LEAVE_STATE_LIST          =  2, //!< User input is focused on the buy/sell/trade list
		LEAVE_STATE_INFO          =  3, //!< User input is focused on the detailed view of a selected object or trade
		LEAVE_STATE_TOTAL         =  4
	};

	//! \brief States that determine what transaction list is active and should be displayed
	enum ACTIVE_LIST {
		ACTIVE_LIST_INVALID  = -1,
		ACTIVE_LIST_BUY      =  0,
		ACTIVE_LIST_SELL     =  1,
		ACTIVE_LIST_TRADE    =  2,
		ACTIVE_LIST_TOTAL    =  3
	};

public:
	LeaveInterface();

	~LeaveInterface();

	//! \brief Not used by this interface as all initialization is performed in the class constructor and MakeActive() method
	void Initialize()
		{}

	//! \brief Processes the buy/sell/trade lists and determines the counts and other information about each transaction
	void MakeActive();

	//! \brief No actions need to take place when a transaction occurs
	void TransactionNotification()
		{}

	//! \brief Handles user input and internal state management
	void Update();

	//! \brief Draws the visible displays, text, and GUI objects to the screen
	void Draw();

private:
	//! \brief Stores the active state of the leave interface
	LEAVE_STATE _state;

	//! \brief Keeps track of which type of list (buy/sell/trade) is currently active
	ACTIVE_LIST _active_list;

	//! \brief The total number of objects marked for purchase
	uint32 _buy_count;

	//! \brief The number of unique objects marked for purchase (e.g. buying four healing potions count as one unique purchase)
	uint32 _buy_unique;

	//! \brief The total number of objects marked for sale
	uint32 _sell_count;

	//! \brief The number of unique objects marked for sale (e.g. selling four healing potions count as one unique sale)
	uint32 _sell_unique;

	//! \brief The total number of marked trades
	uint32 _trade_count;

	//! \brief The number of characters that will transact in at least one trade
	uint32 _trade_characters;

	//! \brief Displays "Purchases" in different text styles based on whether or not the buy list is in view
	hoa_video::TextImage _buy_header;

	//! \brief Displays the buy count/unique data below the buy header
	hoa_video::TextImage _buy_stats;

	//! \brief Displays "Sales" in different text styles based on whether or not the sell list is in view
	hoa_video::TextImage _sell_header;

	//! \brief Displays the sell count/unique data below the sell header
	hoa_video::TextImage _sell_stats;

	//! \brief Displays "Trades" in different text styles based on whether or not the trade list is in view
	hoa_video::TextImage _trade_header;

	//! \brief Displays the trade count/characters data below the trade header
	hoa_video::TextImage _trade_stats;

	//! \brief List header text for the name field
	hoa_video::TextImage _name_header;

	//! \brief List header text for the list of object properties (refer to the BuyListDisplay class)
	hoa_gui::OptionBox _properties_header;

	//! \brief Text to display in the middle window when the selected transaction list is empty
	hoa_video::TextImage _empty_list_text;

	//! \brief A display of all wares marked for purchase and their properties
	BuyListDisplay* _buy_list_display;

	//! \brief A display of all inventory objects marked for sale and their properties
	SellListDisplay* _sell_list_display;

	// TODO: Add once trade interface is working and this class is defined
	// TradeListDisplay* _trade_list_display;

	//! \brief Text that prompts the user for their desired action
	hoa_video::TextImage _main_prompt;

	//! \brief The list of actions that the user can take
	hoa_gui::OptionBox _main_actions;

private:
	/** \brief Changes the current state and modifies other members and display properties appropriately
	*** \param new_state The new state to change the leave interface to
	**/
	void _ChangeState(LEAVE_STATE new_state);

	//! \brief Changes the active transaction list and updates the transaction header texts
	void _CycleActiveTransactionList();
}; // class LeaveInterface : public ShopInterface

} // namespace private_shop

} // namespace hoa_shop

#endif // __SHOP_LEAVE_HEADER__

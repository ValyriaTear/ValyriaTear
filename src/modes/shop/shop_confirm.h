///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    shop_confirm.h
*** \author  Tyler Olsen, roots@allacrost.org
*** \brief   Header file for confirm interface of shop mode
***
*** \note The contents of this file are near identical to the contents of
*** shop_leave.h. When making any changes to this file, please look to shop_leave.h
*** to see if it should have similar changes made.
*** ***************************************************************************/

#ifndef __SHOP_CONFIRM_HEADER__
#define __SHOP_CONFIRM_HEADER__

#include "defs.h"
#include "utils.h"

#include "video.h"
#include "global.h"

#include "shop_utils.h"

namespace hoa_shop {

namespace private_shop {

/** ****************************************************************************
*** \brief The interface where the player confirms and completes their transaction
***
*** After the player has selected their purchases, sales, and trades, they typically
*** enter this interface to finalize their choices and complete the transaction. There
*** are three primary actions that a player may chose from while in this interface.
***
*** -# "Modify Order": allows the player to view the buy/sell/trade lists and change order quantities
*** -# "Clear Order": brings up a second confirmation that will clear all marked purchases, sales, and trades
*** -# "Complete Transaction": makes all purchases, sales, and trades as marked and can not be undone
***
*** When "Modify Order" is selected, the cursor moves up to the current buy/sell/trade list being displayed.
*** The user can increase or decrease selected quantities in buy/sell lists, including setting the
*** quantity to zero, nullifying the purchase/sale. Shop objects with a zero quantity will be removed from
*** the buy/sell lists only after the confirm interface is no longer active, so the player can change their
*** mind and change the quantity back to a non-zero value without having to go back to the buy/sell interface
*** to re-add it. Trades work differently than buy/sell since there is no quantity associated with a trade.
*** Trades can be nullified and when this is done the behavior is similar to a zero quantity in that the
*** trade can be denullified within the confirm interface. Which display list is in view can be changed by
*** the player and the active display will have its header text in a more distinguished text style (header names
*** include "Purchases", "Sales", or "Trades"). The buy/sell/trade count data are also modified and displayed
*** in real time based on the player's actions of changing quantity amounts. The player can also view detailed
*** information about a selected item in the same manner that can be done in the buy and sell interfaces.
*** ***************************************************************************/
class ConfirmInterface : public ShopInterface {
	//! \brief States of the confirm interface used to determine how to process user input and what information to draw
	enum CONFIRM_STATE {
		CONFIRM_STATE_INVALID       = -1,
		CONFIRM_STATE_MAIN          =  0, //!< User input is focused on the main interface prompt
		CONFIRM_STATE_CLEAR         =  1, //!< User input is focused on the second confirmation prompt
		CONFIRM_STATE_LIST          =  2, //!< User input is focused on the buy/sell/trade list
		CONFIRM_STATE_INFO          =  3, //!< User input is focused on the detailed view of a selected object or trade
		CONFIRM_STATE_TOTAL         =  4
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
	ConfirmInterface();

	~ConfirmInterface();

	//! \brief Not used by this interface as all initialization is performed in the class constructor and MakeActive() method
	void Initialize()
		{}

	//! \brief Processes the buy/sell/trade lists and determines the counts and other information about each transaction
	void MakeActive();

	//! \brief Resets all buy/sell/trade count stats and text and clears the list displays
	void TransactionNotification();

	//! \brief Handles user input and internal state management
	void Update();

	//! \brief Draws the visible displays, text, and GUI objects to the screen
	void Draw();

private:
	//! \brief Stores the active state of the confirm interface
	CONFIRM_STATE _state;

	//! \brief Keeps track of which type of list (buy/sell/trade) is currently active
	ACTIVE_LIST _active_list;

	//! \brief True when there are no marked purchase, sale, or trade transactions
	bool _no_transactions;

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

	//! \brief Text displayed in the lower window when there are no marked transactions of any type
	hoa_video::TextImage _no_transactions_text;

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

	//! \brief Confirmation message for when the user selects the "Clear Order" action
	hoa_video::TextImage _clear_prompt;

	//! \brief Confirmation actions for when the user selects the "Clear Order" action
	hoa_gui::OptionBox _clear_actions;

private:
	/** \brief Changes the current state and modifies other members and display properties appropriately
	*** \param new_state The new state to change the confirm interface to
	**/
	void _ChangeState(CONFIRM_STATE new_state);

	//! \brief Changes the active transaction list and updates the transaction header texts
	void _CycleActiveTransactionList();

	//! \brief An update helper function used to change the buy list selection or selected object quantity
	void _UpdateBuyList();

	//! \brief An update helper function used to change the sell list selection or selected object quantity
	void _UpdateSellList();

	//! \brief An update helper function used to change the trade list selection or selected trade
	void _UpdateTradeList();

	/** \brief Changes the buy quantity of a selected object and updates and re-renders buy stats text
	*** \param less_or_more False to decrease the quantity, true to increase it
	*** \param amount The amount to decrease/increase the quantity by (default value == 1)
	*** \return False if no quantity change could take place, true if a quantity change did occur
	*** \note This function signature is identical to the function of the same name as the BuyListDisplay class.
	*** This method serves as a wrapper to that class with the additional feature of automatically updating
	*** buy stats data and re-rendering the stats text as appropriate.
	**/
	bool ChangeBuyQuantity(bool less_or_more, uint32 amount = 1);

	/** \brief Changes the sell quantity of a selected object and updates and re-renders buy stats text
	*** \param less_or_more False to decrease the quantity, true to increase it
	*** \param amount The amount to decrease/increase the quantity by (default value == 1)
	*** \return False if no quantity change could take place, true if a quantity change did occur
	*** \note This function signature is identical to the function of the same name as the SellListDisplay class.
	*** This method serves as a wrapper to that class with the additional feature of automatically updating
	*** sell stats data and re-rendering the stats text as appropriate.
	**/
	bool ChangeSellQuantity(bool less_or_more, uint32 amount = 1);

	//! \brief Re-renders the text image for buy stats using the current buy data
	void _RenderBuyStats();

	//! \brief Re-renders the text image for sell stats using the current sell data
	void _RenderSellStats();

	//! \brief Re-renders the text image for trade stats using the current trade data
	void _RenderTradeStats();

	//! \brief Executes a clear order command from the player, clearing all marked transactions
	void _ClearOrder();

	//! \brief Performs necessary clean-ups to interface state information and marked transactions when the player leaves this interface
	void _LeaveInterface();
}; // class ConfirmInterface : public ShopInterface

} // namespace private_shop

} // namespace hoa_shop

#endif // __SHOP_CONFIRM_HEADER__

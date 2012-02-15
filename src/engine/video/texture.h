///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    tex_mgmt.h
*** \author  Raj Sharma, roos@allacrost.org
*** \brief   Header file for texture management code
***
*** We use texture management so that at runtime, we can load many small images
*** (e.g. tiles) and stick them together into larger textures called "texture
*** sheets". This improves performance, because then we don't have to constantly
*** switch textures while rendering.
***
*** This file contains several classes:
***
*** - <b>TexSheet</b>: represents a large OpenGL texture which is shared among
*** multiple texture objects (images). Abstract to allow for derivative classes
*** to define how they manage their texture objects.
***
*** - <b>FixedTexSheet</b>: a texture sheet for fixed-size textures, i.e. 32x32
*** This class can do all of its operations in O(1) time because it knows in
*** advance that all textures are the same size.
***
*** - <b>FixedTexNode</b>: represents a texture node entry for the
*** FixedTexSheet class.
***
*** - <b>VariableTexSheet</b>: a texture sheet for variable-size textures.
*** This sheet allows textures of any size to be inserted, but has slower
*** performance than the FixedTexSheet.
***
*** - <b>VariableTexNode</b>: represents a texture node entry for the
*** VariableTexSheet class.
*** ***************************************************************************/

#ifndef __TEXTURE_HEADER__
#define __TEXTURE_HEADER__

#ifdef _VS
	#include <GL/glew.h>
#endif

// OpenGL includes
#ifdef __APPLE__
	#include <OpenGL/gl.h>
	#include <OpenGL/glu.h>
#else
	#include <GL/gl.h>
	#include <GL/glu.h>
#endif

#include "defs.h"
#include "utils.h"

namespace hoa_video {

namespace private_video {

//! \brief Used to indicate an invalid texture ID
const GLuint INVALID_TEXTURE_ID = 0xFFFFFFFF;

//! \brief Represents the different image sizes that a texture sheet can hold
enum TexSheetType {
	VIDEO_TEXSHEET_INVALID = -1,

	VIDEO_TEXSHEET_32x32 = 0,
	VIDEO_TEXSHEET_32x64 = 1,
	VIDEO_TEXSHEET_64x64 = 2,
	VIDEO_TEXSHEET_ANY = 3,

	VIDEO_TEXSHEET_TOTAL = 4
};


/** ****************************************************************************
*** \brief An OpenGL texture which can store multiple smaller textures in itself
***
*** The purpose of texture sheets is to save computation resources on texture
*** switches, so that an increased performance can be achieved. TexSheet is
*** an abstract class because the inner textures are stored via different means
*** (fixed size versus variable size textures), so the specific implementation
*** of the inner texture management is defined in a derived class.
***
*** \note This is called TexSheet instead of Texture, so that it is clear that
*** this doesn't represent a texture that you would draw on the screen, but
*** is rather a container for smaller textures.
*** ***************************************************************************/
class TexSheet {
public:
	/** \brief Constructs a new texture sheet
	*** \param sheet_width The width of the sheet
	*** \param sheet_height The height of the sheet
	*** \param sheet_id The OpenGL texture ID value for the sheet
	*** \param sheet_type The type of texture data that the texture sheet should hold
	*** \param sheet_static Whether the sheet should be labeled static or not
	**/
	TexSheet(int32 sheet_width, int32 sheet_height, GLuint sheet_id, TexSheetType sheet_type, bool sheet_static);

	virtual ~TexSheet();

	// ---------- Public methods

	/** \brief Adds a new texture to the tex sheet
	*** \param img A pointer to the new image to add
	*** \param data The image's pixel data to place in the sheet
	*** \return Success/failure
	***
	*** \note The BaseTexture object which is passed into this function will have its
	*** properties modified once it is successfully added to the texture sheet.
	**/
	virtual bool AddTexture(BaseTexture* img, ImageMemory& data) = 0;

	/** \brief Inserts a new texture into the tex sheet
	*** \param img A pointer to the new image to insert
	*** \return Success/failure
	***
	*** The difference between this function and the AddTexture function is that the
	*** texture sheet image data is not modified by this function (i.e., pixel data
	*** is not copied to the texture sheet like it is with AddImage). All it does
	*** is allocate space for the texture pointer to use. Therefore, a call to this
	*** function is usually followed by a call to a function which will modify the
	*** image data at the inserted textures location (CopyRect, CopyScreenRect).
	**/
	virtual bool InsertTexture(BaseTexture* img) = 0;

	/** \brief Removes an image texture from the texture sheet's memory manager
	*** \param img The image to remove
	**/
	virtual void RemoveTexture(BaseTexture* img) = 0;

	/** \brief Marks the texture as free
	*** \param img The image to mark as free
	*** \note Marking an image as free does not delete it. The image may be later
	*** restored from the free state so that it does not have to be re-fetched
	*** from the hard disk.
	**/
	virtual void FreeTexture(BaseTexture* img) = 0;

	/** \brief Restores a texture which was previously freed
	*** \param img The image to mark as used
	**/
	virtual void RestoreTexture(BaseTexture* img) = 0;

	//! \brief Returns the number of textures that are contained on this texture sheet
	virtual uint32 GetNumberTextures() = 0;

	/** \brief Unloads all texture memory used by OpenGL for this sheet
	*** \return Success/failure
	**/
	bool Unload();

	/** \brief Reloads all the images into the sheet and reallocates OpenGL memory
	*** \return Success/failure
	**/
	bool Reload();

	/** \brief Copies pixel data of an image over to a sub-rectangle in the texture sheet
	*** \param x X coordinate of the texture sheet where to copy the pixel data to
	*** \param y Y coordinate of the texture sheet where to copy the pixel data to
	*** \param data The pixel data to copy
	*** \return Success/failure
	***
	*** \note Take extreme care when using this function, as it does not bother to check
	*** whether it is overwriting occupied space within the texture sheet. This can lead
	*** to image corruption. It also does not make any attempt to indicate that the copied
	*** area is now occupied; that must be done externally by the caller (through the use
	*** of creating a new BaseTexture class).
	**/
	bool CopyRect(int32 x, int32 y, private_video::ImageMemory& data);

	/** \brief Copies a portion of the current contents of the screen into the texture sheet
	*** \param x X coordinate of rectangle to copy screen to
	*** \param y Y coordinate of rectangle to copy screen to
	*** \param screen_rect The portion of the screen to copy
	*** \return Success/failure
	***
	*** \note Take extreme care when using this function, as it does not bother to check
	*** whether it is overwriting occupied space within the texture sheet. This can lead
	*** to image corruption. It also does not make any attempt to indicate that the copied
	*** area is now occupied; that must be done externally by the caller (through the use
	*** of creating a new BaseTexture class).
	**/
	bool CopyScreenRect(int32 x, int32 y, const ScreenRect &screen_rect);

	/** \brief Enables (GL_LINEAR) or disables (GL_NEAREST) smoothing for this texture sheet
	*** \param flag True enables smoothing while false disables it. Default value is true.
	**/
	void Smooth(bool flag = true);

	/** \brief Draws the entire texture sheet to the screen
	*** This is used for debugging, as it draws all images contained within the texture to the screen.
	*** It ignores any blending or lighting properties that are enabled in the VideoManager
	**/
	void DEBUG_Draw() const;

	// ---------- Public members

	//! \brief The width and height of the texsheet
	int32 width, height;

	//! \brief The interger that OpenGL uses to refer to this texture
	GLuint tex_id;

	//! \brief The type (dimensions) of images that this texture sheet holds
	TexSheetType type;

	//! \brief If true, images in this sheet that are unlikely to change
	bool is_static;

	//! \brief True if this texture sheet is currently set to GL_LINEAR
	bool smoothed;

	//! \brief Flag indicating if texture sheet is loaded or not
	bool loaded;

protected:
	//! \brief The width and height of the sheet in number of texture blocks
	int32 _block_width, _block_height;
}; // class TexSheet


/** ****************************************************************************
*** \brief Represents a node in a linked list of texture blocks
***
*** This class is used by the FixedTexSheet class to manage its allocated
*** texture blocks.
*** ***************************************************************************/
class FixedTexNode {
public:
	//! \brief The image that belongs to the block
	BaseTexture* image;

	//! \brief The next node in the list
	FixedTexNode* next;

	//! \brief The block index
	int32 block_index;
}; // class FixedTexNode


/** ****************************************************************************
*** \brief Used to manage texture sheets which are designated for fixed image sizes.
***
*** An example where this class would be used would be a 512x512 pixel sheet that
*** only holds 32x32 pixel tiles. The texture sheet's size must be divisible by
*** the size of the images that it holds. For example, you can't create a 256x256
*** sheet which holds tiles which are 17x93.

	*** The open list keeps track of which blocks of memory are open. Note that
	*** we track blocks with both an array and a list. Although it takes up
	*** more memory, this makes all operations dealing with the blocklist
	*** O(1) so that performance is awesome. Memory isn't too bad either,
	*** since the block list is fairly small.

	*** The open list keeps track of which blocks of memory are open. The tail
	*** pointer is also kept so that we can add newly freed blocks to the end
	*** of the list. That way, essentially blocks that are freed are given a
	*** little bit of time from the time they're freed to the time they're
	*** removed, in case they are loaded again in the near future.
*** ***************************************************************************/
class FixedTexSheet : public TexSheet {
public:
	/** \brief Constructs a new memory manager for a texture sheet
	*** \param tex_sheet The texture sheet which this object will manage
	*** \param img_width The width of the images which will be stored in this sheet
	*** \param img_height The height of the images which will be stored in this sheet
	**/
	/** \brief Constructs a new texture sheet
	*** \param sheet_width The width of the sheet
	*** \param sheet_height The height of the sheet
	*** \param sheet_id The OpenGL texture ID value for the sheet
	*** \param sheet_type The type of texture data that the texture sheet should hold
	*** \param sheet_static Whether the sheet should be labeled static or not
	*** \param block_width The width of the texture blocks which will be stored in this sheet
	*** \param block_height The height of the texture blocks which will be stored in this sheet
	***
	*** \note The block_width and block_height parameters must evenly divide into the sheet_width and
	*** sheet_height parameters. Otherwise the constructor will throw an exception
	**/
	FixedTexSheet(int32 sheet_width, int32 sheet_height, GLuint sheet_id, TexSheetType sheet_type, bool sheet_static, int32 img_width, int32 img_height);

	~FixedTexSheet();

	//! \name Methods inherited from TexSheet
	//@{
	bool AddTexture(BaseTexture* img, ImageMemory& data);

	bool InsertTexture(BaseTexture* img);

	void RemoveTexture(BaseTexture* img);

	void FreeTexture(BaseTexture* img);

	void RestoreTexture(BaseTexture* img);

	uint32 GetNumberTextures();
	//@}

private:
	//! \brief The width and height of each texture block, in number of pixels
	int32 _texture_width, _texture_height;

	//! \brief Head of the list of open texture blocks
	FixedTexNode* _open_list_head;

	//! \brief Tail of the list of open memory blocks
	FixedTexNode* _open_list_tail;

	/** \brief A pointer to an array of blocks which is indexed like a 2D array
	*** For example, blocks[x + y * width]->image would tell us which image is
	*** currently allocated at spot (x,y).
	**/
	FixedTexNode* _blocks;

	/** \brief Grabs the block index based off of the image
	*** \param img The image to look for
	*** \return The block index for that image
	**/
	int32 _CalculateBlockIndex(BaseTexture* img);

	/** \brief Adds a node onto the tail of the open list
	*** \param node A pointer to the node to add to the open list
	**/
	void _AddOpenNode(FixedTexNode* node);

	/** \brief Removes a node from the head of the open list to be used
	*** \return A pointer to the node to use, or NULL if no nodes were available
	**/
	FixedTexNode* _RemoveOpenNode();
}; // class FixedTexSheet : public TexSheet


/** ****************************************************************************
*** \brief Keeps track of which images are used/freed in the variable texture mem manager
*** ***************************************************************************/
class VariableTexNode {
public:
	VariableTexNode() :
		image(NULL), free_image(true) {}

	//! \brief A pointer to the image
	BaseTexture* image;

	//! \brief Set to true if the image is freed
	bool free_image;
}; // class VariableTexNode


/** ****************************************************************************
*** \brief Used to manage texture sheets of variable image sizes
***
*** This class divides a texture sheet up into 16x16 pixel blocks, and keeps
*** track of which images have allocated which blocks. This is done to reduce
*** the time complexity for doing texture insertion and removal operations, but
*** the downside is that it may leave some space in the texture sheet occupied
*** but unused for images that are not divisible by 16 in width or height.
*** ***************************************************************************/
class VariableTexSheet : public TexSheet {
public:
	/** \brief Constructs a new texture sheet
	*** \param sheet_width The width of the sheet
	*** \param sheet_height The height of the sheet
	*** \param sheet_id The OpenGL texture ID value for the sheet
	*** \param sheet_type The type of texture data that the texture sheet should hold
	*** \param sheet_static Whether the sheet should be labeled static or not
	**/
	VariableTexSheet(int32 sheet_width, int32 sheet_height, GLuint sheet_id, TexSheetType sheet_type, bool sheet_static);

	~VariableTexSheet();

	//! \name Methods inherited from TexSheet
	//@{
	bool AddTexture(BaseTexture* img, ImageMemory& data);

	bool InsertTexture(BaseTexture* img);

	void RemoveTexture(BaseTexture* img);

	void FreeTexture(BaseTexture* img)
		{ _SetBlockProperties(img, img, true); }

	void RestoreTexture(BaseTexture* img)
		{ _SetBlockProperties(img, img, false); }

	uint32 GetNumberTextures()
		{ return _textures.size(); }
	//@}

private:
	/** \brief The list of 16x16 pixel blocks in the sheet.
	*** The size of this structure is: (width / 16) * (height / 16)
	**/
	VariableTexNode* _blocks;

	/** \brief A set containing each texture that has been inserted into this class
	*** This container is used to be able to quickly determine if a texture is loaded by an object of this class
	**/
	std::set<BaseTexture*> _textures;

	/** \brief Updates the properties of all of the blocks associated with a given texture
	*** \param tex The texture to update
	*** \param new_tex The texture pointer to set the block to
	*** \param new_image The boolean value to set the free status flag to
	**/
	void _SetBlockProperties(BaseTexture* tex, BaseTexture* new_tex, bool free);
}; // class VariableTexSheet : public TexSheet

}  // namespace private_video

}  // namespace hoa_video

#endif // __TEXTURE_HEADER__

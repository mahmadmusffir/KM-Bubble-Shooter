/*
 *  scorescrollnode.cpp
 *  kattMickisShooter
 *
 *  Created by Erik Andersson on 2009-12-26.
 *  Copyright 2009 Åva gymnasium. All rights reserved.
 *
 */

#include <iostream>
#include <sstream>
#include <algorithm>

#include "SDL.h"
#include "SDL_ttf/SDL_ttf.h"

#include "screen.h"
#include "mysdl.h"

#include "menu.h"
#include "scorescrollnode.h"

ScoreScrollNode::ScoreScrollNode(Screen *screen): Node(screen)
{	
	_image = NULL;
	
	_xpos = 175;
	_ypos = 30;
	_w = 140;
	_h = 135;
	
	_name_color.r = _name_color.g = _name_color.b = 0xFF;
	_score_color.r = 0xF1;
	_score_color.g = 0x72;
	_score_color.b = 0xB7;
	
	_font = load_font(16);
	
	_scroll_rect.x = 0;
	_scroll_rect.y = 0;
	_scroll_rect.w = _w;
	_scroll_rect.h = _h;
	
	_bg_color.r = _bg_color.b = 0;
	_bg_color.g = 0;
	
	_font_height = TTF_FontHeight(_font);
}

void ScoreScrollNode::draw(SDL_Surface *surface)
{
	if ( _scores.size() == 0 )
		return;
	
	// the area on which we draw ourselves
	SDL_Rect rect = _screen->makeRect(_xpos, _ypos);
	rect.w = _w;
	rect.h = _h;
	
	// unit height is the height of one score element
	int unit_height = _font_height * 2;
	
	int start_score = _scroll_rect.y / unit_height;
	
	// Check bounds for end_score
	int end_score = _h / unit_height + 2 + start_score;
	end_score = (end_score > _scores.size()) ? _scores.size() : end_score;
	
	SDL_Rect position, cp_position;
	position.x = rect.x;
	position.y = start_score * unit_height - _scroll_rect.y + rect.y - 20;
	
	SDL_SetClipRect(surface, &rect);
	
	score_triplet_list::iterator iter, to;
	iter = _scores.begin() + start_score;
	to = _scores.begin() + end_score;
				 
	for ( ; iter != to; iter++ )
	{
		SDL_Surface **surfaces = (*iter);

		// Blit place
		position.y += _font_height;
		cp_position = position;
		SDL_BlitSurface(surfaces[0], NULL, surface, &cp_position);
		
		// Name
		position.x += surfaces[0]->clip_rect.w;
		cp_position = position;
		SDL_BlitSurface(surfaces[1], NULL, surface, &cp_position);
		
		// Place
		position.x = rect.x;
		position.y += _font_height;
		cp_position = position;
		SDL_BlitSurface(surfaces[2], NULL, surface, &cp_position);
	}
	
	SDL_SetClipRect(surface, NULL);
}

void ScoreScrollNode::renderText()
{
	// Init variables
	SDL_Surface *name, *points, *place, *surface;
	surface = _screen->game()->buffer();
	
	Highscore *hs;
	std::stringstream str, str_place;
	
	// The texts position on text
	SDL_Rect position;
	position.x = 0;
	
	// Get scores
	highscore_list &scores = _screen->menu()->manager.get();
	
	// Create surface on which to draw stuff
	int num_scores = scores.size();
	int height = num_scores * 2 * _font_height;
	
	_height = height;
	
	// Kill old stuff
	for ( score_triplet_list::iterator iter = _scores.begin(); iter != _scores.end(); iter++ )
	{
		SDL_Surface **surfaces = (*iter);
		SDL_FreeSurface(surfaces[0]);
		SDL_FreeSurface(surfaces[1]);
		SDL_FreeSurface(surfaces[2]);
		delete [] surfaces;
	}
	
	_scores.erase(_scores.begin(), _scores.end());
	
	int i = 0;
	for ( highscore_list::iterator iter = scores.begin(); iter != scores.end(); iter++, i++ )
	{
		hs = *iter;
		str << hs->score << " points";
		str_place << (i + 1) << ". ";
		
		// Render texts
		place = TTF_RenderText_Blended(_font, str_place.str().c_str(), _name_color);
		name = TTF_RenderUNICODE_Blended(_font, hs->name, _name_color);
		points = TTF_RenderText_Blended(_font, str.str().c_str(), _score_color);
		
		SDL_Surface **surfaces = new SDL_Surface*[3];
		surfaces[0] = place;
		surfaces[1] = name;
		surfaces[2] = points;
		_scores.push_back(surfaces);
		
		str.str("");
		str_place.str("");
	}
}

void ScoreScrollNode::onshow()
{
	renderText();
}

void ScoreScrollNode::ondrag(int relx, int rely)
{
	_scroll_rect.y += -rely;
	
	if ( _scroll_rect.y > _height - _h )
		_scroll_rect.y = _height - _h;
	
	if ( _scroll_rect.y < 0 )
		_scroll_rect.y = 0;
}

void ScoreScrollNode::onscroll(bool up)
{
	ondrag(0, (up ? 10 : -10));
}
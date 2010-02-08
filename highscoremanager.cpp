/*
 *  highscoremanager.cpp
 *  kattMickisShooter
 *
 *  Created by Erik Andersson on 2009-12-23.
 *  Copyright 2009 Åva gymnasium. All rights reserved.
 *
 */

#include <iostream>
#include <fstream>
#include <iomanip>
#include <iomanip>

#include "highscoremanager.h"

using std::fstream;
using std::ofstream;

HighscoreManager::HighscoreManager() {}

void HighscoreManager::load()
{
	// First we clear all old scores
	for ( highscore_list::iterator iter = _scores.begin(); iter != _scores.end(); iter++ )
		delete (*iter);
	
	fstream fs("scores", fstream::in);
	
	if ( fs.is_open() )
	{
		Highscore hs, *hsobj;
		while ( fs.read((char *) &hs, sizeof hs) )
		{
			hsobj = new Highscore;
			*hsobj = hs;
			_scores.push_back(hsobj);
		}
		
		fs.close();
	}
	
	_scores.sort(comp_highscore);
}

void HighscoreManager::save()
{
	ofstream fs("scores", fstream::trunc);
	
	if ( fs.is_open() )
	{
		for ( highscore_list::iterator iter = _scores.begin(); iter != _scores.end(); iter++ )
		{
			Highscore hs = (**iter);
			fs.write((char *) &hs, sizeof hs);
		}
	}
	
	fs.close();
}

void HighscoreManager::add(Highscore &score)
{
	Highscore *s = new Highscore;
	*s = score;
	
	_scores.push_back(s);
	
	_scores.sort(comp_highscore);
	
	if ( _scores.size() >= HIGHSCORES_SAVED )
	{
		// Remove last score
		Highscore *old = _scores.back();
		_scores.pop_back();
		
		delete old;
	}
}

bool HighscoreManager::isHighscore(long score)
{
	if ( _scores.size() < HIGHSCORES_SAVED )
		return true;
	
	_scores.sort(comp_highscore);
	highscore_list::iterator iter = _scores.begin();
	advance(iter, HIGHSCORES_SAVED - 1); // Go to last score
	
	if ( score > (*iter)->score )
		return true;
	return false;
}

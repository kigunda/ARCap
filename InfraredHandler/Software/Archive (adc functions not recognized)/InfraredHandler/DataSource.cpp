/*
 * DataSource.cpp
 *
 *  Created on: 2014-03-01
 *      Author: kigunda
 */

#include "DataSource.h"

// ALLOCATION
DataSource::DataSource() {}
DataSource::~DataSource() {}

// LISTENERS

/*
 * Adds a listener queue to this data source.
 * @param queue - the queue to add
 */
void DataSource::addListener(OS_EVENT *queue) {
	listeners.push_back(queue);
}

/*
 * Removes a listener queue from this data source.
 * @param queue - the queue to remove
 */
void DataSource::removeListener(OS_EVENT *queue) {
	listeners.remove(queue);
}


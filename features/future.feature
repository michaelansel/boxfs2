Feature: ???

In order to maintain the appearance of a local file system
As a Box.com user with a large number of files and folders
I want to defer loading data until it is needed.
I want to prefetch data in the background.
I want to reuse recently accessed data.
I want remote changes to my account reflected locally.
I want to always show the latest changes.

Background:
	Given my Box account is mounted to "/tmp/boxfs"

Scenario: 

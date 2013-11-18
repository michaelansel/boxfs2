Feature: Simple access to files

In order to allow simple integration with existing applications
As a Box user
I want my Box account presented through a standard file system interface.

Background:
	Given my Box account is mounted to "/tmp/boxfs"

Scenario: List the contents of a directory
	When I run "ls /tmp/boxfs"
	Then the output should include all my root folders

Scenario: Create a new directory
	When I run "mkdir -p /tmp/boxfs/a-new-folder"
	Then my Box account should have a root folder called "a-new-folder"
	When I run "ls /tmp/boxfs"
	Then the output should include "a-new-folder"

Scenario: Recursively remove a directory
	Given I have a file "a-new-folder/a-new-file" in my Box account
	When I run "rm -rf /tmp/boxfs/a-new-folder"
	Then my Box account should not have a root folder called "a-new-folder"
	When I run "ls /tmp/boxfs"
	Then the output should not include "a-new-folder"

Scenario: Read the contents of a file

Scenario: Create a new files

Scenario: Remove an existing file

Scenario: Modify an existing file

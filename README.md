# capacity4dev to capacity4more migration


This package of modules is used to import the data of the Drupal 6 version of [capacity4dev](http://capacity4dev.ec.europa.eu) into an installation of the [capacity4more](https://github.com/capacity4dev/capacity4more) (Drupal 7) distribution.

This package requires a local working copy of the capacity4dev environment + modules to export the data.

This package is open sourced so it can be used as an example how to import data from other platform(s) into the capacity4more distribution.


## Installation

This package is only used during the data migration. 
No need to add this to the make file of the capacity4more distribution.


### Clone repositories

Clone the capacity4more distribution.

	$ git clone https://github.com/capacity4dev/capacity4more.git
	
Clone the c4d_import package outside the cloned capacity4more distribution.

	$ git clone https://github.com/capacity4dev/c4d_import.git
	

### Create the capacitymore configuration file

Configure the capacity4more distribution development environment. Start by creating a copy of the example file.

Fill in the configuration parameters & credentials.


### Add import package to capacity4more

Add the import package to the repository by adding following configuration to the config.sh script of capacitymore:

	##
	# External folders or files that need to be symlinked into the www folder
	# AFTER the make files have been processed.
	# The variable is an array, add each with an unique index number.
	# Each line should contain the source path > target path.
	# The target path needs to be relative to the www folder (Drupal root).
	##
	SYMLINKS[0]="/PATH/TO/capacity4more-migrate/c4d_migrate>sites/all/modules/c4d_migrate"

	##
	# Commands to run after the installation is finished.
	#
	# This will enable & configure the c4d_migrate module.
	##
	function post_install {
	  cd $ROOT/www

	  echo "Enable and confgure the c4d_migrate module."
	  drush -y en c4d_migrate
	  drush vset c4d_migrate_db_database "DBNAME"
	  drush vset c4d_migrate_db_hostname "127.0.0.1"
	  drush vset c4d_migrate_db_password "PASSWORD"
	  drush vset c4d_migrate_db_username "USERNAME"
	  drush vset c4d_migrate_files_root "/PATH/TO/OLD/WEBSITE/ROOT"

	  cd $ROOT
	}

	##
	# Commands to run after the upgrade is finished.
	#
	# Make sure that the c4d_migrate module is added.
	##
	function post_upgrade {
	  post_install
	}

	##
	# Commands to run after the reset is is finished.
	#
	# Make sure that the c4d_migrate module is added.
	##
	function post_reset {
	  post_install
	}

This will:

* Create a symlink to the repository.
* Enable the c4d_migrate module.
* Set the credentials & file directory of a local copy of the capacity4dev environment.

### Install or upgrade

Run the install script in case of a fresh installation:

	$ ./install
	
Or run the upgrade script, this will add the c4d_import package and enable the c4d_migrate module.

	$ ./upgrade



## Usage

Import the data trough the web interface or command line using Drush.

List the available import scripts:

	drush ms
	
Run one of the migration scripts with information about the results:

	drush mi --instrument --feedback="30 seconds" C4dMigrateImportNodeArticle
	
Or run all at once:

	drush mi --instrument --feedback="30 seconds" --all

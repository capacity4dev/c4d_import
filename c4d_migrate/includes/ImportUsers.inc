<?php
/**
 * @file
 * Migrate class to import users.
 */

/**
 * Class C4dMigrateImportUsers
 */
class C4dMigrateImportUsers extends C4dMigrateImportBase {
  public $entityType = 'user';

  /**
   * Content creations need to be run before this one.
   *
   * @var array
   */
  public $dependencies = array(
    'C4dMigrateImportRoles',
  );

  /**
   * Configure the object during creation.
   */
  public function __construct() {
    parent::__construct(
          MigrateGroup::getInstance(C4D_MIGRATE_MIGRATION_GROUP)
    );

    $this->description = t('Import users from our Drupal 6 database.');

    $query = $this->createSelect('c4d_export_user_profile', 'u')
                  ->orderBy('uid', 'ASC')
                  ->fields('u');

    $this->source = new MigrateSourceSQL($query, array(), NULL, array('map_joinable' => FALSE));

    // Create a map object for tracking the relationships between source rows.
    $key = array(
      'uid' => array(
        'type'     => 'varchar',
        'length'   => 255,
        'not null' => TRUE,
      ),
    );

    // Automatically convert drupal passwords to drupal7 hash.
    $destination_handler = new MigrateDestinationUser(array('md5_passwords' => TRUE));

    $this->map = new MigrateSQLMap(
      $this->machineName,
      $key,
      $destination_handler->getKeySchema()
    );

    $this->destination = $destination_handler;

    $this->addUserFieldMapping();
  }

  /**
   * {@inheritdoc}
   */
  public function addUserFieldMapping() {
    $this->addFieldMapping('name', 'name')->dedupe('users', 'name');
    $this->addFieldMapping('mail', 'mail')->dedupe('users', 'mail');
    $this->addFieldMapping('pass', 'pass');
    $this->addFieldMapping('signature', 'signature')->defaultValue('');
    $this->addFieldMapping('created', 'created');
    $this->addFieldMapping('access', 'access');
    $this->addFieldMapping('login', 'login');
    $this->addFieldMapping('status', 'status');
    $this->addFieldMapping('timezone', 'timezone_name')->defaultValue(NULL);
    $this->addFieldMapping('language', 'language')->defaultValue('');
    $this->addFieldMapping('picture', 'picture')->defaultValue(0);
    $this->addFieldMapping('init', 'init');
    $this->addFieldMapping('data', 'data');
    $this->addFieldMapping('c4m_body', 'field_description')
         ->arguments(array('format' => 'full_html'));
    $this->addFieldMapping('c4m_first_name', 'field_firstname');
    $this->addFieldMapping('c4m_last_name', 'field_lastname');
    $this->addFieldMapping('c4m_organisation', 'field_organisation');
    $this->addFieldMapping('c4m_organisation_type', 'field_organisation_type_of');
    $this->addFieldMapping('c4m_vocab_topic', 'field_ref_topic')
      ->arguments(array('source_type' => 'tid'));
    $this->addFieldMapping('c4m_vocab_topic_expertise', 'field_ref_topic_expertise')
      ->arguments(array('source_type' => 'tid'));
    $this->addFieldMapping('c4m_country', 'field_country');
    // geo_vocab was not added to the user profile in Drupal 6.
    $this->addFieldMapping('c4m_vocab_geo', 'taxonomy')
         ->arguments(array('source_type' => 'tid'));

    // Special field mapping for files.
    $this->addFileFieldMapping('c4m_media', 'field_picture');

    // @TODO Group memberships and roles.

    $this->addFieldMapping('roles', 'roles')
         ->defaultValue(DRUPAL_AUTHENTICATED_RID);
  }

  /**
   * Prepare the row data loaded from the Drupal 6 database.
   *
   * @param object $row
   *   The data from the CSV file.
   *
   * @return bool
   *   Success.
   */
  public function prepareRow($row) {
    parent::prepareRow($row);

    $row->data = unserialize($row->data);

    $topic_nids = unserialize($row->field_ref_topic);
    $topics = array();

    foreach($topic_nids as $nid) {
      $title = $this->getNodeTitleFromNid($nid);

      $mapped = c4m_vocab_topic_get_mapping_by_source($title);

      foreach($mapped as $term) {
        $topics[] = $term->tid;
      }
    }

    $row->field_ref_topic = $topics;

    $topic_nids = unserialize($row->field_ref_topic_expertise);
    $topics = array();

    foreach($topic_nids as $nid) {
      $title = $this->getNodeTitleFromNid($nid);

      $mapped = c4m_vocab_topic_get_mapping_by_source($title);

      foreach($mapped as $term) {
        $topics[] = $term->tid;
      }
    }

    $row->field_ref_topic_expertise = $topics;

    // Add the picture file data.
    $this->prepareFileInRow($row, 'field_picture');

    // Add the roles.
    $roles = array();
    foreach (unserialize($row->roles) as $role) {
      $roles[] = $this->handleSourceMigration('C4dMigrateImportRoles', $role['rid'], NULL, $this);
    }
    $row->roles = $roles;

    $row->taxonomy = unserialize($row->taxonomy);
    $row->taxonomy = $this->mapTaxonomyVocabWithParent(
      $this->mapTaxonomyVocabRegionCountries($row->taxonomy, 'c4m_vocab_geo')
    );

    // Else login timestamp is set to <CURRENT> (timestamp of migration).
    if ($row->login == 0) {
      $row->login = $row->access;
    }

    return TRUE;
  }

  /**
   * Review a data row after fetch, returning FALSE to skip it.
   *
   * @param object $row
   *   The data row we are importing.
   */
  public function prepare($user, $row) {
    if ($row->uid == 1) {
      $destination_name = 'admin';
      $destination_uid  = 1;
      if ($destination_uid) {
        $user->uid  = $destination_uid;
        $user->name = $destination_name;
      }
    }
  }

  /**
   * Perform a rollback operation - remove migrated items from the destination.
   */
  protected function rollback() {
    $return    = MigrationBase::RESULT_COMPLETED;
    $itemlimit = $this->getItemLimit();
    $idlist    = $this->getOption('idlist');
    if ($idlist) {
      // Make the IDs keys, to more easily identify them.
      $idlist = array_flip(explode(',', $idlist));
    }

    if (method_exists($this->destination, 'bulkRollback')) {
      // Too many at once can lead to memory issues, so batch 'em up.
      $destids     = array();
      $sourceids   = array();
      $batch_count = 0;
      foreach ($this->map as $destination_key) {
        if ($this->timeOptionExceeded()) {
          break;
        }
        if (($return = $this->checkStatus()) != MigrationBase::RESULT_COMPLETED) {
          break;
        }

        if ($itemlimit && ($this->total_processed + $batch_count >= $itemlimit)) {
          break;
        }

        $current_source_key = $this->map->getCurrentKey();

        // If there's an idlist, skip anything not in the list.
        if (($idlist && !isset($idlist[$current_source_key['sourceid1']])) || $current_source_key['sourceid1'] == 1) {
          continue;
        }

        // Note that bulk rollback is only supported for single-column keys.
        $sourceids[] = $current_source_key;
        if (!empty($destination_key->destid1)) {
          $map_row = $this->map->getRowByDestination((array) $destination_key);
          if ($map_row['rollback_action'] == MigrateMap::ROLLBACK_DELETE && $destination_key->sourceid !== 1) {
            $destids[] = $destination_key->destid1;
          }
        }

        $batch_count++;
        if ($batch_count >= $this->rollbackBatchSize) {
          try {
            if ($this->systemOfRecord == Migration::SOURCE) {
              if (!empty($destids)) {
                migrate_instrument_start('destination bulkRollback');
                $this->destination->bulkRollback($destids);
                migrate_instrument_stop('destination bulkRollback');
              }
            }
            // Keep track in case of interruption.
            migrate_instrument_start('rollback map/message update');
            $this->map->deleteBulk($sourceids);
            migrate_instrument_stop('rollback map/message update');
            $this->total_successes += $batch_count;
            $this->successes_since_feedback += $batch_count;
          }
          catch (Exception $e) {
            $this->handleException($e, FALSE);
            migrate_instrument_stop('bulkRollback');
            migrate_instrument_stop('rollback map/message update');
          }
          $destids   = array();
          $sourceids = array();
          // Will increment even if there was an exception... But we don't
          // really have a way to know how many really were successfully
          // rolled back.
          $this->total_processed += $batch_count;
          $this->processed_since_feedback += $batch_count;
          $batch_count = 0;
        }
      }
      if ($batch_count > 0) {
        if ($this->systemOfRecord == Migration::SOURCE) {
          if (!empty($destids)) {
            migrate_instrument_start('destination bulkRollback');
            $this->destination->bulkRollback($destids);
            migrate_instrument_stop('destination bulkRollback');
          }
          $this->total_processed += $batch_count;
          $this->total_successes += $batch_count;
          $this->processed_since_feedback += $batch_count;
          $this->successes_since_feedback += $batch_count;
        }
        migrate_instrument_start('rollback map/message update');
        $this->map->deleteBulk($sourceids);
        migrate_instrument_stop('rollback map/message update');
      }
    }
    else {
      foreach ($this->map as $destination_key) {
        if ($this->timeOptionExceeded()) {
          break;
        }
        if (($return = $this->checkStatus()) != MigrationBase::RESULT_COMPLETED) {
          break;
        }
        if ($this->itemOptionExceeded()) {
          break;
        }
        $current_source_key = $this->map->getCurrentKey();

        // If there's an idlist, skip anything not in the list.
        if ($idlist && !isset($idlist[$current_source_key['sourceid1']])) {
          continue;
        }

        // Rollback one record.
        try {
          if ($this->systemOfRecord == Migration::SOURCE) {
            // Skip when the destination key is null.
            $skip = FALSE;
            foreach ($destination_key as $key_value) {
              if (is_null($key_value)) {
                $skip = TRUE;
                break;
              }
            }
            if (!$skip) {
              $map_row = $this->map->getRowByDestination((array) $destination_key);
              if ($map_row['rollback_action'] == MigrateMap::ROLLBACK_DELETE) {
                migrate_instrument_start('destination rollback');
                $this->destination->rollback((array) $destination_key);
                migrate_instrument_stop('destination rollback');
              }
            }
          }

          migrate_instrument_start('rollback map/message update');
          $this->map->delete($current_source_key);
          migrate_instrument_stop('rollback map/message update');
          $this->total_successes++;
          $this->successes_since_feedback++;
        }
        catch (Exception $e) {
          // TODO: At least count failures.
          continue;
        }
        $this->total_processed++;
        $this->processed_since_feedback++;
      }
    }

    $this->map->clearMessages();

    $this->progressMessage($return);

    // If we're using highwater marks, reset at completion of a full rollback.
    // TODO: What about partial rollbacks? Probably little we can do to make
    // that work cleanly...
    if ($this->highwaterField) {
      $this->saveHighwater('', TRUE);
    }

    return $return;
  }
}

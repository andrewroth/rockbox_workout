class CreateSetLogEntries < ActiveRecord::Migration
  def self.up
    create_table :set_log_entries do |t|
      t.integer :exercise_log_entry_id
      t.integer :workout_set_id
      t.integer :position
      t.decimal :v1
      t.decimal :v2
      t.decimal :v3
      t.decimal :v4
      t.decimal :v5
      t.decimal :v6
      t.decimal :v7

      t.timestamps
    end
  end

  def self.down
    drop_table :set_log_entries
  end
end

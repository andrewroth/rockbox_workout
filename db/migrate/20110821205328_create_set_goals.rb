class CreateSetGoals < ActiveRecord::Migration
  def self.up
    create_table :set_goals do |t|
      t.integer :workout_date_id
      t.integer :set_id
      t.string :variable
      t.float :value

      t.timestamps
    end
  end

  def self.down
    drop_table :set_goals
  end
end

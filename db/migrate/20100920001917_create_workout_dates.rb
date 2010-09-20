class CreateWorkoutDates < ActiveRecord::Migration
  def self.up
    create_table :workout_dates do |t|
      t.datetime :when
      t.integer :workout_id

      t.timestamps
    end
  end

  def self.down
    drop_table :workout_dates
  end
end

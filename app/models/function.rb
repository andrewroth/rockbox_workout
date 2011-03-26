class Function < ActiveRecord::Base
  include Csv
  belongs_to :exercise_set
  belongs_to :workout

  def selwrite_csv
    super do
      WorkoutDate.where("finished_at IS NULL").collect(&:workout).flatten.collect(&:exercises).flatten.collect(&:exercise_sets).flatten.collect(&:functions).uniq
    end
  end

  def value(n)
    if ((min_n.present? && n >= min_n) || !min_n.present?) &&
      ((max_n.present? && n <= max_n) || !max_n.present?)
      value = base.to_i + (n * inc.to_i)
      if round_to_nearest.present?
        r = round_to_nearest
        value = ((value + r / 2.0).to_i / r) * r
      end
      return value
    else
      return nil
    end
  end
end

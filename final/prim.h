struct prim
{
	using node_id_t = size_type;
	using result_t  = vector<std::pair<node_id_t, node_id_t>>;

	result_t
	operator()(const graph& g, node_id_t src)
	{
		constexpr double inf = std::numeric_limits<double>::infinity();
		const auto edge_from = [&](node_id_t s) { return g.table_[s].related_; };
		const auto   deleted = [&](node_id_t s) { return g.table_[s].deleted_; };
		size_type          n = g.table_.size();

		result_t ret;

		vector<double   >       min(n, inf  );
		vector<node_id_t>      from(n, 0    );
		vector<bool     > confirmed(n, false);

		min[src] = 0; from[src] = src; confirmed[src] = true;
		for (const auto& e : edge_from(src))
        {
             min[e.to_] = e.weight_;
            from[e.to_] = e.from_; 
        }

		while (true)
		{
			int             p = -1;
			double min_weight = inf;

			for (node_id_t i = 0; i < n; ++i)
			{
				if (  deleted(i)) continue;
				if (confirmed[i]) continue;
				if (min[i] < min_weight)   { p = i; min_weight = min[i]; }
			}

			if (p == -1) break;

			confirmed[p] = true;
			ret.push_back(std::make_pair(from[p], p));

			for (const auto& e : edge_from(p)) if (e.weight_ < min[e.to_])
			{
				 min[e.to_] = e.weight_;
				from[e.to_] = e.from_;
			}
		}

		return ret;
	}
};

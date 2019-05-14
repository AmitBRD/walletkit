package com.breadwallet.crypto.blockchaindb.apis;

import com.breadwallet.crypto.blockchaindb.BlockchainCompletionHandler;
import com.breadwallet.crypto.blockchaindb.BlockchainDb;
import com.breadwallet.crypto.blockchaindb.errors.QueryError;
import com.breadwallet.crypto.blockchaindb.errors.QueryModelError;
import com.breadwallet.crypto.blockchaindb.models.Transaction;
import com.google.common.base.Optional;
import com.google.common.collect.ImmutableListMultimap;
import com.google.common.collect.Multimap;

import org.json.JSONArray;
import org.json.JSONObject;

import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Semaphore;

import static com.google.common.base.Preconditions.checkArgument;

public class TransactionJsonApi {

    private static final int PAGINATION_COUNT = 5000;

    private final JsonApiClient jsonClient;
    private final ExecutorService executorService;

    public TransactionJsonApi(JsonApiClient jsonClient, ExecutorService executorService) {
        this.jsonClient = jsonClient;
        this.executorService = executorService;
    }


    public void getTransactions(String id, List<String> addresses, long beginBlockNumber, long endBlockNumber,
                                boolean includeRaw, boolean includeProof,
                                BlockchainCompletionHandler<List<Transaction>> handler) {
        executorService.submit(() -> getTransactionsOnExecutor(id, addresses, beginBlockNumber, endBlockNumber,
                includeRaw, includeProof, handler));
    }

    public void getTransaction(String id, boolean includeRaw, boolean includeProof,
                               BlockchainCompletionHandler<Transaction> handler) {
        // TODO: I don't think we should be building it like this
        String path = String.format("transactions/%s", id);
        Multimap<String, String> params = ImmutableListMultimap.of("include_proof", String.valueOf(includeProof),
                "include_raw", String.valueOf(includeRaw));

        jsonClient.makeRequest(path, params, new JsonApiCompletionObjectHandler() {
            @Override
            public void handleData(JSONObject json, boolean more) {
                checkArgument(!more);
                Optional<Transaction> transaction = Transaction.asTransaction(json);
                if (transaction.isPresent()) {
                    handler.handleData(transaction.get());
                } else {
                    handler.handleError(new QueryModelError("Transform error"));
                }
            }

            @Override
            public void handleError(QueryError error) {
                handler.handleError(error);
            }
        });
    }

    private void getTransactionsOnExecutor(String id, List<String> addresses, long beginBlockNumber, long endBlockNumber,
                                           boolean includeRaw, boolean includeProof,
                                           BlockchainCompletionHandler<List<Transaction>> handler) {
        final QueryError[] error = {null};
        List<Transaction> allTransactions = new ArrayList<>();
        Semaphore sema = new Semaphore(0);

        ImmutableListMultimap.Builder<String, String> paramBuilders = ImmutableListMultimap.builder();
        paramBuilders.put("blockchain_id", id);
        paramBuilders.put("include_proof", String.valueOf(includeProof));
        paramBuilders.put("include_raw", String.valueOf(includeRaw));
        for (String address : addresses) paramBuilders.put("address", address);

        for (long i = beginBlockNumber; i < endBlockNumber && error[0] == null; i += PAGINATION_COUNT) {
            paramBuilders.put("start_height", String.valueOf(beginBlockNumber));
            paramBuilders.put("end_height", String.valueOf(Math.min(beginBlockNumber + PAGINATION_COUNT,
                    endBlockNumber)));

            jsonClient.makeRequest("transactions", paramBuilders.build(), new JsonApiCompletionArrayHandler() {
                @Override
                public void handleData(JSONArray json, boolean more) {
                    Optional<List<Transaction>> transactions = Transaction.asTransactions(json);
                    if (transactions.isPresent()) {
                        allTransactions.addAll(transactions.get());
                    } else {
                        error[0] = new QueryModelError("Transform error");
                    }

                    sema.release();
                }

                @Override
                public void handleError(QueryError error) {
                    sema.release();
                }
            });

            sema.acquireUninterruptibly();
        }

        if (error[0] != null) {
            handler.handleError(error[0]);
        } else {
            handler.handleData(allTransactions);
        }
    }
}

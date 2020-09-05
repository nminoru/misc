package jp.nminoru.jsql_parser_test;

import net.sf.jsqlparser.JSQLParserException;
import net.sf.jsqlparser.parser.CCJSqlParserUtil;
import net.sf.jsqlparser.statement.Statements;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import static org.junit.Assert.*;


public class MarkLogicTest {

    @Before
    public void setUp() throws Exception {
    }

    @After
    public void tearDown() throws Exception {
    }    

    @Test
    public void test() throws Exception {
        Statements stmt = CCJSqlParserUtil.parseStatements("SELECT * FROM employees WHERE employees MATCH 'Manager'");
        
        assertNotNull(stmt);
        System.out.println("***");
    }
}

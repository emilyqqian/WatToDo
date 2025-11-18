import { useState } from 'react'
import { Navigate, useNavigate } from 'react-router-dom'
import {
  Box,
  Button,
  Container,
  Stack,
  Typography,
} from '@mui/material'
import { useGlobal } from '../SessionManager'
import BoardSection from '../Components/BoardSection'
import TaskboardDialog from '../Components/NewBoardDialogue'
import { addTaskboard } from '../APIManager'

function Home() {
  const { state, updateState } = useGlobal()
  const navigate = useNavigate()

  if (!state.loggedIn) {
    return <Navigate to="/login" replace />
  }

  const [isDialogOpen, setIsDialogOpen] = useState(false)

  const openDialog = () => setIsDialogOpen(true)
  const closeDialog = () => setIsDialogOpen(false)

  const handleSaveBoard = ({ name }) => {
    addTaskboard(name, state.user.userId).then(data => {
      if (data == null) return;

      let tmp = state.privateTaskboardList;
      tmp.push({
        taskboard_id: data.taskboard_id,
        name: name,
        tasks: [],
        users: [
          {
            userId: state.user.userId,
            username: state.user.username,
            isAdmin: true
          }
        ]
      })
      updateState({privateTaskboardList: tmp})
    })
  }

  const handleBoardClick = (board) => {
    updateState({ currentTaskBoard: board })
    navigate('/board')
  }

  return (
    <Container maxWidth="lg" sx={{ py: 6 }}>
      <Stack spacing={6}>
        <Box
          sx={{
            display: 'flex',
            flexDirection: { xs: 'column', md: 'row' },
            justifyContent: 'space-between',
            alignItems: { xs: 'flex-start', md: 'center' },
            gap: 2,
          }}
        >
          <Box>
            <Typography variant="h3" fontWeight={700}>
              Wat to Do Dashboard
            </Typography>
            <Typography variant="body1" color="text.secondary" sx={{ mt: 1 }}>
              Organize your personal focus and collaborate on shared priorities.
            </Typography>
          </Box>
          <Button
            variant="contained"
            size="large"
            onClick={openDialog}
            sx={{ alignSelf: { xs: 'stretch', md: 'center' } }}
          >
            + New Board
          </Button>
        </Box>

        <BoardSection
          title="Personal Task Boards"
          boards={state.privateTaskboardList}
          onBoardClick={handleBoardClick}
        />
        <BoardSection
          title="Shared Task Boards"
          boards={state.sharedTaskboardList}
          onBoardClick={handleBoardClick}
        />
      </Stack>

      <TaskboardDialog
        open={isDialogOpen}
        onClose={closeDialog}
        onSave={handleSaveBoard}
      />
    </Container>
  )
}

export default Home